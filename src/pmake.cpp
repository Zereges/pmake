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



pmake::pmake(const std::vector<std::string>& makefile, pmake_options&& options) : m_options(std::move(options))
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
            replace(s, "$@", record.get_target());
            replace(s, "$?", record.get_dependencies_recent());
            replace(s, "$^", record.get_dependencies_stripped());
            replace(s, "$+", record.get_dependencies());
            replace(s, "$<", record.get_dependencies().empty() ? "" : record.get_dependencies().front().get_name());
            record.add_command(move(s));
        }
    }
}

int pmake::run(const std::string& exe_name)
{

    return CODE_SUCCESS;
}
