#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <regex>
#include "pmake.hpp"
#include "pmake_options.hpp"
#include "main.hpp"
using namespace std;

int main(int argc, char* argv[])
{
    pmake_options options;

    string line, prev, file("Makefile");
    ifstream stream(file);
    vector<string> makefile;
    while (getline(stream, line))
    {
        if (prev != "")
        {
            prev += line;
            if (!line.length() || prev[prev.length() - 1] != '\\' || (line.length() > 1 && prev[prev.length() - 2] == '\\'))
            {
                makefile.push_back(move(prev));
                prev = "";
                continue;
            }
            prev.pop_back();
        }
        if (!line.length()) continue;
        if (line[line.length() - 1] == '\\' && line.length() > 1 && line[line.length() - 2] != '\\')
        {
            prev = line;
            prev.pop_back();
            continue;
        }
        makefile.push_back(move(line));
    }
    if (prev != "")
        cerr << "Unexpected end of file" << endl;

    pmake make(makefile);

    cout << endl;
}
