#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include "makefile_records.hpp"
#include "mutex.hpp"
#include "condition_variable.hpp"
#include "thread.hpp"
#include "main.hpp"

/**!
    \brief Base class representing how make processes targets.
*/
class pmake
{
    public:
        using thread_type = my_thread<process_states(pmake*, makefile_record&)>;
        using pmake_variables = std::unordered_map<std::string, std::string>;

        //! Default empty constructor.
        pmake() : m_condvar(m_mutex) { }

        //! Defaulted move constructor.
        pmake(pmake&&) = default;

        //! Defaulted move assignment.
        pmake& operator=(pmake&&) = default;

        //! Deleted copy constructor.
        pmake(const pmake&) = delete;

        //! Deleted copy assignment.
        pmake& operator=(const pmake&) = delete;

        //! Constructor, allowing partialy parsed makefile, command line options, and exe_name to be passed.
        pmake(const std::vector<std::string>& makefile, pmake_options&& options, std::string&& exe_name);

        //! Adds new variable. If value represented by key already is in list, it's value is redefined.
        //! \param name name of the variable.
        //! \param value value of the variable.
        void add_variable(const std::string& name, const std::string& value) { m_variables[name] = value; }

        //! Checks whether given variable is in list.
        //! \param name variable to be found.
        //! \return true if such variable exists, false otherwise.
        bool is_variable(const std::string& name) const { return m_variables.end() != m_variables.find(name); }

        //! Gets value of variable by name.
        //! \param name name of the variable.
        //! \return const reference to value of the variable.
        const std::string& get_variable(const std::string& name) const { return m_variables.at(name); }

        //! Constructs new record at the end of pmake::m_records.
        //! \param targets std::vector of names of targets.
        //! \param dependencies std::vector of names of dependencies.
        //! \param options const reference to \ref pmake_options.
        void add_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies, const pmake_options& options)
        {
            m_records.emplace_back(std::move(targets), std::move(dependencies), options);
        }

        //! Gets executable name for POSIX like output.
        //! \return const reference to executable name.
        const std::string& get_exe_name() const { return m_exe_name; }

        //! Starts processing targets.
        int run();
        
        static const std::regex var_def; //!< Regex matching variable definition.
        static const std::regex var_use; //!< Regex matching variable use.
        static const std::regex target_def; //!< Regex matching rule definition
        static const std::regex command_def; //!< Regex matching command definition
        static const std::regex item_def; //!< Regex matching single item in rule.
    
    private:
        pmake_options m_options; //!< Options given on command line.
        std::string m_exe_name; //!< Executable name for POSIX like error output.
        pmake_variables m_variables; //!< Variables and their values.
        makefile_records m_records; //!< Records in Makefile
        
        mutex m_mutex; //!< Mutex to synchronize multiple threads.
        condition_variable m_condvar; //< Condition variable to synchronize multiple threads.

        //! Replaces all variable uses in input.
        //! \param input line, where to search for variable uses.
        //! \return new constructed string, where all occurences of variable uses in input are dereferenced.
        std::string replace_occurences(const std::string& input);

        //! Checks, whether exists circularity of some rules in makefile. Is called whenever new record is added.
        //! \param target newly added target.
        //! \param deps newly added dependencies.
        std::string is_circular(const file& target, const std::vector<file>& deps);

        //! Recursively processes a \ref makefile_record by according to make specification.
        //! \param record record to be processed.
        //! \return return value of given task.
        process_states process_target(makefile_record& record);

        //! Static version of #process_target.
        //! \sa process_target(makefile_record&)
        //! \param mak pointer to \ref pmake object.
        //! \param record record to be processed.
        //! \return return value of given task.
        static process_states static_process_target(pmake* mak, makefile_record& record);
};
