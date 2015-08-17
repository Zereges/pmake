#include <string>
#include <iostream>
#include <getopt.h>
#include <stdio.h>
#include <unordered_map>
#include "pmake_options.h"
using namespace std;

void print_version()
{
    cout << "1.0, Filip Kliber <zereges@gmail.com>";
}

void print_help(const string& exe_name)
{
    cout << "Usage: " << exe_name << " [options] [target] ..." << endl <<
        R"--(Options:
    -b, -m
        These options are ignored for compatibility with other versions of make.

    -B, --always-make
        Unconfitionally make all targets.

    -d, --verbose
        Print debugging information about what is going on.

    -C dir, --directory=dir
        Change to directory dir before doing anything, Sunsequent uses of this
        argument will be relative to previous one.

    -f file
        Use file as Makefile.

    -j jobs, --jobs=jobs
        Specifies the number jobs (commands) to run simultaneously. This is
        implicitely on with jobs=number of cores.

    -n, --just-print, --dry-run, --recon
        Print the commands that would be executed, but do not execute them.

    -q, --question
        "Question mode". Do not run any commands, or print anything; just return
        an exit status that is zero if the specified targets are already up
        to date, nonzero otherwise.

    -v, --version
        Print a message containing version and author. After the message is
        printed, pmake will exit.

    -W file, --what-if=file, --new-file=file, --assume-new=file
        Pretend that the target file has just been  modified. It is almost the
        same as running a touch command on the given file before running make,
        except that the modification time is changed only in the imagination of
        make.

    --warn-undefined-variables
        Warn when an undefined variable is referenced.)--" << endl << endl;

        print_version();
}

int main(int argc, char* argv[])
{
    pmake_options options;
    string exe_name = argv[0];
    while (true)
    {
        static struct option long_options[] =
        {
            {              "always-make",       no_argument, 0, 'B' },
            {                  "verbose",       no_argument, 0, 'd' },
            {                "directory", required_argument, 0, 'C' },
            {                     "jobs", required_argument, 0, 'j' },
            {               "just-print",       no_argument, 0, 'n' },
            {                  "dry-run",       no_argument, 0, 'n' },
            {                    "recon",       no_argument, 0, 'n' },
            {                 "question",       no_argument, 0, 'q' },
            {                  "version",       no_argument, 0, 'v' },
            {                  "what-if", required_argument, 0, 'W' },
            {                  "dry-run", required_argument, 0, 'W' },
            {                    "recon", required_argument, 0, 'W' },
            {                     "help",       no_argument, 0, 'h' },
            { "warn-undefined-variables",       no_argument, 0,  0  },
            { 0, 0, 0, 0}
        };
        int option_index = 0;

        int c = getopt_long(argc, argv, "bmBdC:fj::nqvW:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
        case 0:
            if (long_options[option_index].flag == 0)
            {
                if (long_options[option_index].name == "warn-undefined-variables")
                    options.set_warn_undefined();
            }
            break;

        case 'b':
        case 'm':
            // ignored arguments
            break;
        case 'B':
            options.set_always_make()
            break;
        case 'd':
            options.set_verbose()
            break;
        case 'C':
            options.set_dirs(optarg);
            break;
        case 'f':
            options.set_make_file(optarg);
            break;
        case 'j':
            try
            {
                options.set_jobs(stoi(optarg));
                break;
            }
            catch (const invalid_argument&)
            {
                cerr << "jobs value must be valid positive integer" << endl;
            }
            catch (const out_of_range&)
            {
                cerr << "jobs value must be valid positive integer" << endl;
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
            return EXIT_SUCCESS;
        case 'h':
            print_help(exe_name);
            return EXIT_SUCCESS;

        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc)
        while (optind < argc)
            options.set_target(argv[optind++])
}
