#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <getopt.h>
#ifdef _WIN32
    #include <direct.h>
    #define chdir _chdir // win chdir is deprecated
#else
    #include <unistd.h>
    #include <fcntl.h>
#endif
#include <sys/stat.h>
#include "main.hpp"
#include "pmake.hpp"
#include "pmake_options.hpp"

int main(int argc, char* argv[])
{

    pmake_options options;
    std::string exe_name = argv[0];
    for (;;)
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
            if (chdir(optarg))
            {
                std::cerr << exe_name << ": " << optarg << ": No such file or directory. Stop." << std::endl;
                return CODE_FAILURE;
            }
            break;
        case 'f':
        {
            struct stat buf;
            if (!stat(optarg, &buf))
                options.set_make_file(optarg);
            else
                std::cerr << exe_name << ": " << optarg << ": no such file. Ignoring." << std::endl;
            break;
        }
        case 'j':
        {
            int jobs = to_number(optarg);
            if (jobs > 0)
                options.set_jobs(jobs);
            else
            {
                std::cerr << exe_name << ": the '-j' option requires a positive integer argument." << std::endl;
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

    if (options.is_verbose())
        std::cout << "Verbose: Finished parsing command line options." << std::endl;

    std::string line, prev;
    if (options.is_verbose())
        std::cout << "Verbose: Opening makefile: '" << options.get_makefile() << "'." << std::endl;

#ifdef _WIN32 // to maintain compatibility
    std::ifstream stream(options.get_makefile());
    if (stream.fail())
    {
        std::cerr << exe_name << ": " << options.get_makefile() << ": No such file. Stop." << std::endl;
        return CODE_FAILURE;
    }
#else // to pass school requirements for POSIX API usage
    int fd;
    std::string file_content;
    if ((fd = open(options.get_makefile().c_str(), O_RDONLY)) >= 0)
    {
        const int BUFSIZE = 1024;
        char buffer[BUFSIZE];
        while (int size = read(fd, buffer, BUFSIZE))
        {
            if (size != BUFSIZE)
                buffer[size] = '\0';
            file_content += buffer;
        }
        cout << s << endl;
    }
    else if (errno == EACCES)
    {
        std::cerr << exe_name << ": " << options.get_makefile() << ": Permission denied. Stop." << std::endl;
        return CODE_FAILURE;
    }
    else if (errno == ENOENT)
    {
        std::cerr << exe_name << ": " << options.get_makefile() << ": No such file. Stop." << std::endl;
        return CODE_FAILURE;
    }
    std::stringstream stream(file_content);
#endif
    std::vector<std::string> makefile;
    if (options.is_verbose())
        std::cout << "Verbose: Merging lines of makefile which end with '\\'." << std::endl;

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
    {
        std::cerr << "Unexpected end of file. Last line of Makefile probably ends with '\\'." << std::endl;
        return CODE_FAILURE;
    }

    pmake make;
    try
    {
        make = pmake(makefile, std::move(options), std::move(exe_name));
    }
    catch (std::invalid_argument&)
    {
        return CODE_FAILURE;
    }

    return make.run();
}
