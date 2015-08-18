#pragma once
#include <vector>
#include <thread>

class pmake_options
{
    public:
        using records = std::vector<std::string>;

        pmake_options() : m_always_make(false), m_verbose(false), m_just_print(false), m_question(false),
            m_warn_undefined(false), m_jobs(std::thread::hardware_concurrency()), m_makefile("Makefile")
        { }

        void set_always_make() { m_always_make = true; }
        void set_verbose() { m_verbose = true; }
        void set_just_print() { m_just_print = true; }
        void set_question() { m_question = true; }
        void set_warn_undefined() { m_warn_undefined = true; }
        void set_jobs(unsigned jobs) { m_jobs = jobs; }
        void set_make_file(const std::string& makefile) { m_makefile = makefile; }
        void set_dirs(const std::string& dir) { m_dirs.push_back(dir); }
        void set_files(const std::string& file) { m_files.push_back(file); }
        void set_targets(const std::string& target) { m_targets.push_back(target); }

        bool is_alwyas_make() { return m_always_make; }
        bool is_verbose() { return m_verbose; }
        bool is_just_print() { return m_just_print; }
        bool is_question() { return m_question; }
        bool is_warn_undefined() { return m_warn_undefined; }

        unsigned get_jobs() { return m_jobs; }
        const std::string& get_makefile() { return m_makefile; }
        const records& get_dirs() { return m_dirs; }
        const records& get_files() { return m_files; }
        const records& get_targets() { return m_targets; }

    private:
        bool m_always_make;
        bool m_verbose;
        bool m_just_print;
        bool m_question;
        bool m_warn_undefined;
        unsigned m_jobs;
        std::string m_makefile;
        records m_dirs;
        records m_files;
        records m_targets;
};
