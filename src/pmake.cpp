#include <vector>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include "main.hpp"
#include "pmake.hpp"
#include "makefile_record.hpp"
#include "pmake_options.hpp"
#include "file.hpp"
using namespace std;

const regex pmake::var_def(R"(([a-zA-Z0-9_-]+) *= *([^ ]+.*))");
const regex pmake::var_use(R"(\$[({]([a-zA-Z0-9_-]+)[)}])");
const regex pmake::target_def(R"(([^:]+):(.*))");
const regex pmake::command_def(R"(^\s*(.+)\s*$)");
const regex pmake::item_def(R"(^\s*(.+)\s*$)");

vector<string> split(const string& str, char delim)
{
    vector<std::string> elems;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delim))
    {
        smatch sm;
        if (regex_match(item, sm, pmake::item_def)) // removing empty strings.
            elems.push_back(sm[1]);
    }

    return move(elems);
}

string& replace(string& what, const string& find, const string& replace)
{
    size_t pos = 0;
    while ((pos = what.find(find, pos)) != std::string::npos)
    {
        what.replace(pos, find.length(), replace);
        pos += replace.length();
    }
    return what;
}

string& replace(string& what, const string& find, const makefile_record::dependencies& replaces)
{
    string s;
    for (const file& dep : replaces)
        s += (dep.get_name() + " ");
    if (s.length())
        s.pop_back();
    return replace(what, find, s);
}

std::string pmake::replace_occurences(const std::string& input)
{
    std::smatch use;
    std::string s = input;
    while (regex_search(s, use, var_use)) // dereferencing variable uses in definition
    {
        bool is_var = is_variable(use[1]);
        s = s.replace(use.position(0), use[0].length(), is_var ? get_variable(use[1]) : "");
        if (!is_var && m_options.is_warn_undefined())
            std::cout << "Warning: variable " << use[0] << " is undefined. (--warn-undefined-variables)" << std::endl;
        use = std::smatch();
    }
    return std::move(s);
}

string pmake::is_circular(const file& target, const vector<file>& dependencies)
{
    for (const file& dep : dependencies)
    {
        try
        {
            auto deps = m_records.find_record(dep).get_dependencies();
            if (std::find(deps.begin(), deps.end(), target) != deps.end())
                return "'" + target.get_name() + "' -> '" + dep.get_name() + "'";
            else
                return is_circular(target, deps);
        }
        catch (invalid_argument&) { }
    }
    return "";
}

pmake::pmake(const std::vector<std::string>& makefile, pmake_options&& options, string&& exe_name) : m_options(move(options)),
    m_exe_name(move(exe_name)),
    m_variables // some of built-in variables according to GNU make man page.
    {
        { "AR", "ar" },
        { "AS", "as" },
        { "CC", "cc" },
        { "CXX", "g++" },
        { "CPP", "cc -E" },
        { "FC", "f77" },
        { "M2C", "m2c" },
        { "PC", "pc" },
        { "CO", "co" },
        { "GET", "get" },
        { "LEX", "lex" },
        { "YACC", "yacc" },
        { "LINT", "lint" },
        { "MAKEINFO", "makeinfo" },
        { "TEX", "tex" },
        { "TEXI2DVI", "texi2dvi" },
        { "WEAVE", "weave" },
        { "CWEAVE", "cweave" },
        { "TANGLE", "tangle" },
        { "CTANGLE", "ctangle" },
        { "RM", "rm -f" },
        { "ARFLAGS", "rv" },
        { "ASFLAGS", "" },
        { "CFLAGS", "" },
        { "CXXFLAGS", "" },
        { "COFLAGS", "" },
        { "CPPFLAGS", "" },
        { "FFLAGS", "" },
        { "GFLAGS", "" },
        { "LDFLAGS", "" },
        { "LDLIBS", "" },
        { "LFLAGS", "" },
        { "YFLAGS", "" },
        { "PFLAGS", "" },
        { "RFLAGS", "" },
        { "LINTFLAGS", "" },
    }
{
    if (m_options.is_verbose())
        cout << "Verbose: Parsing makefile: '" << m_options.get_makefile() << "'." << endl;
    for (size_t i = 0; i < makefile.size(); ++i)
    {
        string str = replace_occurences(makefile[i].substr(0, makefile[i].find('#'))); // remove comments and dereference variables
        smatch sm;
        if (regex_match(str, sm, var_def)) // variable definition
        {
            add_variable(sm[1], sm[2]);
            continue;
        }
        else if (regex_match(str, sm, target_def)) // target definiton
        {
            vector<string> targets = split(sm[1], ' ');
            vector<string> dependencies = split(sm[2], ' ');
            add_record(move(targets), move(dependencies), m_options);
            string circ = is_circular(m_records.back().get_target(), m_records.back().get_dependencies());
            if (circ != "")
            {
                cerr << m_options.get_makefile() << ": Found circular dependency " << circ << "." << endl;
                throw invalid_argument("Circular dependency");
            }
        }
        else if (regex_match(str, sm, command_def))
        {
            if (m_records.size() == 0)
            {
                cerr << m_options.get_makefile() << ": recipe commences before first target. Stop." << endl;
                throw invalid_argument("Missing target");
            }
            string s = sm[1]; // implicit cast
            makefile_record& record = m_records.back();
            replace(s, "$@", record.get_target().get_name());
            replace(s, "$?", record.get_dependencies_recent());
            replace(s, "$^", record.get_dependencies_stripped());
            replace(s, "$+", record.get_dependencies());
            replace(s, "$<", record.get_dependencies().empty() ? "" : record.get_dependencies().front().get_name());
            record.add_command(move(s));
        }
    }
}

int pmake::run()
{
    if (!m_records.size())
    {
        cerr << m_exe_name << ": No targets. Stop." << endl;
        return CODE_FAILURE;
    }
    if (!m_options.get_targets().size()) // use first target if none is specified.
        m_options.set_default_target(m_records.front().get_target());

    for (const file& target : m_options.get_targets())
    {
        try
        {
            switch (process_states state = process_target(m_records.find_record(target)))
            {
            case process_states::UP_TO_DATE:
                if (!m_options.is_question())
                    cout << m_exe_name << ": '" << target.get_name() << "' is up to date." << endl;
                break;
            case process_states::QUESTION_FAILURE:
                return CODE_QUESTION_FAILURE;
            case process_states::BUILD_FAILED:
                return CODE_FAILURE;
            default:
                break;
            }
        }
        catch (invalid_argument&)
        {
            cerr << m_exe_name << ": No rule to build target '" << target.get_name() << "'. Stop" << endl;
            return CODE_FAILURE;
        }
    }

    return CODE_SUCCESS;
}

process_states pmake::process_target(const makefile_record& record)
{
    bool must_rebuild = record.get_dependencies().empty();
    if (m_options.is_verbose())
        cout << "Verbose: Considering target '" << record.get_target().get_name() << "'." << endl;
    for (const file& dependency : record.get_dependencies())
    {
        try
        {
            switch (process_states state = process_target(m_records.find_record(dependency)))
            {
            case process_states::MUST_REBUILD:
                if (m_options.is_verbose())
                    cout << "Verbose: Target '" << record.get_target().get_name() << "' successfuly rebuilt." << endl;
                if (m_options.is_question())
                    return process_states::QUESTION_FAILURE;
                must_rebuild = true;
                break;
            case process_states::QUESTION_FAILURE:
            case process_states::BUILD_FAILED:
                return state; // like throw;
            default:
                break;
            }
        }
        catch (invalid_argument&)
        {
            if (m_options.is_verbose())
                cout << "Verbose: Checking timestamp of dependency '" << dependency.get_name() << "'." << endl;
            if (m_options.is_always_make() || dependency.is_recent(record.get_target()))
            {
                if (m_options.is_verbose())
                {
                    cout << "Verbose: Must rebuild target '" << record.get_target().get_name() << "'.";
                    if (m_options.is_always_make())
                        cout << " Option --always-make specified.";
                    else
                        cout << " Dependency '" << dependency.get_name() << "' is newer than '" << record.get_target().get_name() << "'.";
                    cout << endl;
                }
                must_rebuild = true;
            }
        }
    }
    if (must_rebuild)
    {
        if (m_options.is_question())
            return process_states::QUESTION_FAILURE;
        if (m_options.is_verbose())
            cout << "Verbose: Executing commands for '" << record.get_target().get_name() << "'." << endl;
        if (int ret = record.execute(m_options.is_just_print()))
        {
            cerr << m_exe_name << ": recipe for target '" << record.get_target().get_name() << "' failed. (" << ret << "). Stopping" << endl;
            return process_states::BUILD_FAILED;
        }
    }

    return must_rebuild ? process_states::MUST_REBUILD : process_states::UP_TO_DATE;
}
