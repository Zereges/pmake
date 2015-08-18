#include <vector>
#include <string>
#include <regex>
#include <iostream>
#include "pmake.hpp"
#include "pmake_options.hpp"
using namespace std;

const regex pmake::var_def(R"(([a-zA-Z0-9_-]+) *= *([^ ]+.*))");
const regex pmake::var_use(R"(\$[({]([a-zA-Z0-9_-]+)[)}])");


pmake::pmake(const std::vector<std::string>& makefile, pmake_options&& options) : m_optons(std::move(options))
{

    size_t i;
    for (i = 0; i < makefile.size(); ++i)
    {
        string str = replace_occurences(makefile[i].substr(0, makefile[i].find('#')));
        smatch def;
        if (regex_match(str, def, var_def)) // variable definition
        {
            add_variable(def[1], def[2]);
            continue;
        }
    }
}
