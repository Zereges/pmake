#pragma once
#include <vector>
#include <thread>
#include "file.hpp"

/**!
    \brief Class representing options passed on command line.
    \sa main.hpp#print_help()
*/
class pmake_options
{
    public:
        using records = std::vector<std::string>;
        using targets = std::vector<file>; // problems with makefile_record::targets

        //! Constructs an object with default values.
        pmake_options() : m_always_make(false), m_verbose(false), m_just_print(false), m_question(false),
            m_warn_undefined(false), m_jobs(std::thread::hardware_concurrency()), m_makefile("Makefile")
        { }

        //! Sets -B, --always-make flag on.
        void set_always_make() { m_always_make = true; }

        //! Sets -d, --verbose flag on.
        void set_verbose() { m_verbose = true; }

        //! Sets -n, --just-print, --dry-run, --recon flag on.
        void set_just_print() { m_just_print = true; }

        //! Sets -q, --question flag on.
        void set_question() { m_question = true; }

        //! Sets --warn-undefined-variables flag on.
        void set_warn_undefined() { m_warn_undefined = true; }

        //! Sets -j, --jobs value.
        //! \param jobs Value to be set.
        void set_jobs(unsigned jobs) { m_jobs = jobs; }

        //! Sets -f value.
        //! \param makefile Value to be set.
        void set_make_file(const std::string& makefile) { m_makefile = makefile; }

        //! Sets -W, --what-if, --new-file, --assume-new value
        //! \param file Value to be appended.
        void set_files(const std::string& file) { m_files.push_back(file); }

        //! Sets which targets have to be build.
        //! \param target target to be build.
        void set_targets(std::string&& target) { m_targets.emplace_back(std::move(target)); }


        //! Checks, whether -B, --always-make flag is on.
        //! \return true, if is on, false otherwise.
        bool is_always_make() const { return m_always_make; }

        //! Checks, whether -d, --verbose flag is on.
        //! \return true, if is on, false otherwise.
        bool is_verbose() const { return m_verbose; }

        //! Checks, whether -n, --just-print, --dry-run, --recon flag is on.
        //! \return true, if is on, false otherwise.
        bool is_just_print() const { return m_just_print; }

        //! Checks, whether -q, --question flag is on.
        //! \return true, if is on, false otherwise.
        bool is_question() const { return m_question; }

        //! Checks, whether --warn-undefined-variables flag is on.
        //! \return true, if is on, false otherwise.
        bool is_warn_undefined() const { return m_warn_undefined; }


        //! Gets value representing -j, --jobs.
        //! \return Value of that option.
        unsigned get_jobs() const { return m_jobs; }

        //! Gets value representing -f.
        //! \return Value of that option.
        const std::string& get_makefile() const { return m_makefile; }

        //! Gets value representing -W, --what-if, --new-file, --assume-new.
        //! \return Value of that option.
        const records& get_files() const { return m_files; }

        //! Gets targets to be build given on command line.
        //! \return Targets to be build
        const targets& get_targets() const { return m_targets; }

        //! Sets default target if none is given.
        //! \param target target to be set.
        void set_default_target(const file& target) { m_targets.push_back(target); }

    private:
        bool m_always_make; //!< Represents -B, --always-make flag.
        bool m_verbose; //!< Represents -d, --verbose flag.
        bool m_just_print; //!< Represents -n, --just-print, --dry-run, --recon flag.
        bool m_question; //!< Represents -q, --question flag.
        bool m_warn_undefined; //!< Represents --warn-undefined-variables flag.
        unsigned m_jobs; //!< Represents -j, --jobs value.
        std::string m_makefile; //!< Represents -f value.
        records m_files; //!< Represents -W, --what-if, --new-file, --assume-new value.
        targets m_targets; //!< Represents targets to be build.
};
