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
        }
        else if (regex_match(str, sm, command_def))
        {
            if (m_records.size() == 0)
            {
                cerr << m_options.get_makefile() << ": recipe commences before first target. Stop." << endl;
                throw invalid_argument("Can not recover.");
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
    if (!m_options.get_targets().size())
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
    for (const file& dependency : record.get_dependencies())
    {
        try
        {
            switch (process_states state = process_target(m_records.find_record(dependency)))
            {
            case process_states::MUST_REBUILD:
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
            if (m_options.is_always_make() || dependency.is_recent(record.get_target()))
                must_rebuild = true;
        }
    }
    if (must_rebuild)
        if (int ret = record.execute(m_options.is_just_print()))
        {
            cerr << m_exe_name << ": recipe for target '" << record.get_target().get_name() << "' failed. (" << ret << "). Stopping" << endl;
            return process_states::BUILD_FAILED;
        }

    return must_rebuild ? process_states::MUST_REBUILD : process_states::UP_TO_DATE;
}
