#include <vector>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include "pmake.hpp"
#include "pmake_options.hpp"
using namespace std;

const regex pmake::var_def(R"(([a-zA-Z0-9_-]+) *= *([^ ]+.*))");
const regex pmake::var_use(R"(\$[({]([a-zA-Z0-9_-]+)[)}])");
const regex pmake::target_def(R"(([^:]+):(.*))");
const regex pmake::command_def(R"(^\s*(.+)\s*$)");

vector<string> split(const string& str, char delim)
{
    vector<std::string> elems;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delim))
        elems.push_back(item);

    return move(elems);
}

pmake::pmake(const std::vector<std::string>& makefile, pmake_options&& options) : m_optons(std::move(options))
{
    size_t blocks = 0;
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
            add_record(move(targets), move(dependencies));
            ++blocks;
        }
        else if (regex_match(str, sm, command_def))
        {
            if (blocks == 0)
            {
                cerr << m_optons.get_makefile() << ": recipe commences before first target. Stop." << endl;
                throw invalid_argument("Can not recover.");
            }
            m_records[blocks - 1].add_command(sm[1]);
        }
    }
}
