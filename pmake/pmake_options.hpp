#pragma once
#include <vector>
#include <thread>

class pmake_options
{
    public:
        pmake_options() : m_always_make(false), m_verbose(false), m_just_print(false), m_question(false),
            m_warn_undefined(false), m_jobs(std::thread::hardware_concurrency()), m_make_file("Makefile")
        { }

        void set_always_make() { m_always_make = true; }
        void set_verbose() { m_verbose = true; }
        void set_just_print() { m_just_print = true; }
        void set_question() { m_question = true; }
        void set_warn_undefined() { m_warn_undefined = true; }
        void set_jobs(unsigned jobs) { m_jobs = jobs; }
        void set_make_file(const std::string& make_file) { m_make_file = make_file; }
        void set_dirs(const std::string& dir) { m_dirs.push_back(dir); }
        void set_files(const std::string& file) { m_files.push_back(file); }
        void set_targets(const std::string& target) { m_targets.push_back(target); }

    private:
        bool m_always_make;
        bool m_verbose;
        bool m_just_print;
        bool m_question;
        bool m_warn_undefined;
        unsigned m_jobs;
        std::string m_make_file;
        std::vector<std::string> m_dirs;
        std::vector<std::string> m_files;
        std::vector<std::string> m_targets;
};
