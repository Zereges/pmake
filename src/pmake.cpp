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

const std::regex pmake::var_def(R"(([a-zA-Z0-9_-]+) *= *([^ ]+.*))");
const std::regex pmake::var_use(R"(\$[({]([a-zA-Z0-9_-]+)[)}])");
const std::regex pmake::target_def(R"(([^:]+):(.*))");
const std::regex pmake::command_def(R"(^\s*(.+)\s*$)");
const std::regex pmake::item_def(R"(^\s*(.+)\s*$)");

std::vector<std::string> split(const std::string& str, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(str);
    std::string item;
    while (getline(ss, item, delim))
    {
        std::smatch sm;
        if (std::regex_match(item, sm, pmake::item_def)) // removing empty strings.
            elems.push_back(sm[1]);
    }

    return move(elems);
}

std::string& replace(std::string& what, const std::string& find, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = what.find(find, pos)) != std::string::npos)
    {
        what.replace(pos, find.length(), replace);
        pos += replace.length();
    }
    return what;
}

std::string& replace(std::string& what, const std::string& find, const makefile_record::dependencies& replaces)
{
    std::string s;
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

std::string pmake::is_circular(const file& target, const std::vector<file>& dependencies)
{
    for (const file& dep : dependencies)
    {
        try
        {
            auto deps = m_records.find_record(dep)->get_dependencies();
            if (std::find(deps.begin(), deps.end(), target) != deps.end())
                return "'" + target.get_name() + "' -> '" + dep.get_name() + "'";
            else
                return is_circular(target, deps);
        }
        catch (std::invalid_argument&) { }
    }
    return "";
}

pmake::pmake(const std::vector<std::string>& makefile, pmake_options&& options, std::string&& exe_name) : m_options(std::move(options)),
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
        std::cout << "Verbose: Parsing makefile: '" << m_options.get_makefile() << "'." << std::endl;
    for (size_t i = 0; i < makefile.size(); ++i)
    {
        std::string str = replace_occurences(makefile[i].substr(0, makefile[i].find('#'))); // remove comments and dereference variables
        std::smatch sm;
        if (regex_match(str, sm, var_def)) // variable definition
        {
            add_variable(sm[1], sm[2]);
            continue;
        }
        else if (regex_match(str, sm, target_def)) // target definiton
        {
            std::vector<std::string> targets = split(sm[1], ' ');
            std::vector<std::string> dependencies = split(sm[2], ' ');
            add_record(move(targets), move(dependencies), m_options);
            std::string circ = is_circular(m_records.back().get_target(), m_records.back().get_dependencies());
            if (circ != "")
            {
                std::cerr << m_options.get_makefile() << ": Found circular dependency " << circ << "." << std::endl;
                throw std::invalid_argument("Circular dependency");
            }
        }
        else if (regex_match(str, sm, command_def))
        {
            if (m_records.size() == 0)
            {
                std::cerr << m_options.get_makefile() << ": recipe commences before first target. Stop." << std::endl;
                throw std::invalid_argument("Missing target");
            }
            std::string s = sm[1]; // implicit cast
            makefile_record& record = m_records.back();
            replace(s, "$@", record.get_target().get_name());
            replace(s, "$?", record.get_dependencies_recent());
            replace(s, "$^", record.get_dependencies_stripped());
            replace(s, "$+", record.get_dependencies());
            replace(s, "$<", record.get_dependencies().empty() ? "" : record.get_dependencies().front().get_name());
            record.add_command(std::move(s));
        }
    }
}

int pmake::run()
{
    if (!m_records.size())
    {
        std::cerr << m_exe_name << ": No targets. Stop." << std::endl;
        return CODE_FAILURE;
    }
    if (!m_options.get_targets().size()) // use first target if none is specified.
        m_options.set_default_target(m_records.front().get_target());

    for (const file& target : m_options.get_targets())
    {
        makefile_records::iterator iter = m_records.find_record(target);
        if (iter != m_records.end()) // exists
        {
            switch (/*process_states state = */process_target(*iter))
            {
            case process_states::UP_TO_DATE:
                if (!m_options.is_question())
                    std::cout << m_exe_name << ": '" << target.get_name() << "' is up to date." << std::endl;
                break;
            case process_states::QUESTION_FAILURE:
                return CODE_QUESTION_FAILURE;
            case process_states::BUILD_FAILED:
                return CODE_FAILURE;
            default:
                break;
            }
        }
        else
        {
            std::cerr << m_exe_name << ": No rule to build target '" << target.get_name() << "'. Stop" << std::endl;
            return CODE_FAILURE;
        }
    }

    return CODE_SUCCESS;
}

process_states pmake::process_target(makefile_record& record)
{
    if (record.is_built())
        return process_states::UP_TO_DATE;
    bool must_rebuild = record.get_dependencies().empty();
    if (m_options.is_verbose())
        std::cout << "Verbose: Considering target '" << record.get_target().get_name() << "'." << std::endl;

    std::vector<thread_type> threads;

    for (const file& dependency : record.get_dependencies())
    {
        makefile_records::iterator iter = m_records.find_record(dependency);
        if (iter != m_records.end()) // exists
        {
            threads.emplace_back(&pmake::static_process_target, &(*this), *iter);
        }
        else
        {
            if (m_options.is_verbose())
                std::cout << "Verbose: Checking timestamp of dependency '" << dependency.get_name() << "'." << std::endl;
            if (m_options.is_always_make() || dependency.is_recent(record.get_target()))
            {
                if (m_options.is_verbose())
                {
                    std::cout << "Verbose: Must rebuild target '" << record.get_target().get_name() << "'.";
                    if (m_options.is_always_make())
                        std::cout << " Option --always-make specified.";
                    else
                        std::cout << " Dependency '" << dependency.get_name() << "' is newer than '" << record.get_target().get_name() << "'.";
                    std::cout << std::endl;
                }
                must_rebuild = true;
            }
        }
    }

    for (thread_type& thr : threads)
    {
        if (thr.join() == process_states::MUST_REBUILD)
            must_rebuild = true;
    }

    if (must_rebuild)
    {
        if (m_options.is_question())
            std::exit(CODE_QUESTION_FAILURE);
        if (m_options.is_verbose())
            std::cout << "Verbose: Executing commands for '" << record.get_target().get_name() << "'." << std::endl;
        if (int ret = record.execute(m_options.is_just_print()))
        {
            std::cerr << m_exe_name << ": recipe for target '" << record.get_target().get_name() << "' failed. (" << ret << "). Stopping" << std::endl;
            std::exit(CODE_FAILURE);
        }
    }

    return must_rebuild ? process_states::MUST_REBUILD : process_states::UP_TO_DATE;
}

process_states pmake::static_process_target(pmake* mak, makefile_record& record)
{
    process_states state = mak->process_target(record);
    switch (state)
    {
    case process_states::MUST_REBUILD:
        if (mak->m_options.is_verbose())
            std::cout << "Verbose: Target '" << record.get_target().get_name() << "' successfuly rebuilt." << std::endl;
        break;
    default:
        break;
    }
    return state;
}