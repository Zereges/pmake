#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <getopt.h>
#include "pmake.hpp"
#include "pmake_options.hpp"
#include "main.hpp"
using namespace std;

int main(int argc, char* argv[])
{
    pmake_options options;
    string exe_name = argv[0];
    while (true)
    {
        static struct option long_options[] =
        {
            { "always-make",       no_argument, 0, 'B' },
            { "verbose",       no_argument, 0, 'd' },
            { "directory", required_argument, 0, 'C' },
            { "jobs", required_argument, 0, 'j' },
            { "just-print",       no_argument, 0, 'n' },
            { "dry-run",       no_argument, 0, 'n' },
            { "recon",       no_argument, 0, 'n' },
            { "question",       no_argument, 0, 'q' },
            { "version",       no_argument, 0, 'v' },
            { "what-if", required_argument, 0, 'W' },
            { "dry-run", required_argument, 0, 'W' },
            { "recon", required_argument, 0, 'W' },
            { "help",       no_argument, 0, 'h' },
            { "warn-undefined-variables",       no_argument, 0,  0 },
            { 0, 0, 0, 0 }
        };
        int option_index = 0;

        int c = getopt_long(argc, argv, "bmBdC:f:j:nqvW:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
        case 0:
            if (long_options[option_index].flag == 0)
            {
                if (!strcmp(long_options[option_index].name, "warn-undefined-variables"))
                    options.set_warn_undefined();
            }
            break;

        case 'b':
        case 'm':
            // ignored arguments
            break;
        case 'B':
            options.set_always_make();
            break;
        case 'd':
            options.set_verbose();
            break;
        case 'C':
            options.set_dirs(optarg);
            break;
        case 'f':
            options.set_make_file(optarg);
            break;
        case 'j':
        {
            int jobs = to_number(optarg);
            if (jobs > 0)
                options.set_jobs(jobs);
            else
            {
                cerr << "jobs value must be valid positive integer" << endl;
                return CODE_FAILURE;
            }
            break;
        }
        case 'n':
            options.set_just_print();
            break;
        case 'q':
            options.set_question();
            break;
        case 'W':
            options.set_files(optarg);
            break;

        case 'v':
            print_version();
            return CODE_SUCCESS;
        case 'h':
            print_help(exe_name);
            return CODE_SUCCESS;
        case '?':
            // getopt_long already printed an error message.
            break;
        }
    }

    if (optind < argc)
        while (optind < argc)
            options.set_targets(argv[optind++]);


    string line, prev;
    ifstream stream(options.get_makefile());
    vector<string> makefile;
    while (getline(stream, line)) // handling lines that ends with a backslash
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

    try
    {
        pmake make(makefile, move(options));
    }
    catch (invalid_argument&)
    {
        return CODE_FAILURE;
    }

    cout << endl;
}
