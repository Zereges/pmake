#pragma once
#include <vector>
#include <thread>
#include "file.hpp"

class pmake_options
{
    public:
        using records = std::vector<std::string>;
        using targets = std::vector<file>; // problems with makefile_record::targets

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
        void set_files(const std::string& file) { m_files.push_back(file); }
        void set_targets(std::string&& target) { m_targets.emplace_back(std::move(target)); }

        bool is_alwyas_make() const { return m_always_make; }
        bool is_verbose() const { return m_verbose; }
        bool is_just_print() const { return m_just_print; }
        bool is_question() const { return m_question; }
        bool is_warn_undefined() const { return m_warn_undefined; }

        unsigned get_jobs() const { return m_jobs; }
        const std::string& get_makefile() const { return m_makefile; }
        const records& get_files() const { return m_files; }
        const targets& get_targets() const { return m_targets; }

    private:
        bool m_always_make;
        bool m_verbose;
        bool m_just_print;
        bool m_question;
        bool m_warn_undefined;
        unsigned m_jobs;
        std::string m_makefile;
        records m_files;
        targets m_targets;
};
