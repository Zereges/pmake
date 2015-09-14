#pragma once
#include <vector>
#include <string>
#include "pmake_options.hpp"
#include "file.hpp"
#include "main.hpp"

/**!
    \brief Class for storing single records in Makefile.
    Record in makefile has following syntax:
    target [...] : [dependency] [...]
        [command]
        [...]
    Where commands line have to start with at least one white space character.
*/
class makefile_record
{
    public:
        using dependencies = std::vector<file>;
        using targets = std::vector<file>;

        //! Constructs makefile record with given arguments.
        //! \param targets std::vector of targets.
        //! \param dependencies std::vector of dependencies.
        //! \param options Const reference to options.
        makefile_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies, const pmake_options& options);

        //! Checks, whether all dependencies are more recent than \ref file argument.
        //! \param tar to check against.
        //! \return true if all dependencies of this \ref makefile_record are more recent than tar, false otherwise.
        bool is_recent(const file& tar) const;

        //! Gets default target of this \ref makefile_record.
        //! \return const reference to \ref file object containing target.
        const file& get_target() const { return m_targets.front(); }

        //! Gets dependencies of this \ref makefile_record.
        //! \return const reference to std::vector of \ref file.
        const dependencies& get_dependencies() const { return m_dependencies; }

        //! Checks, whether argument is member of targets of this \ref makefile_record.
        //! \param tar target to look for.
        //! \return true if there is, false otherwise.
        bool has_target(const file& tar) const { return find(m_targets.begin(), m_targets.end(), tar) != m_targets.end(); }

        //! Returns new std::vector of dependencies without duplicities.
        //! \return std::vector of \ref file.
        dependencies get_dependencies_stripped() const;

        //! Returns new std::vector of dependencies more recent than default target.
        //! \return std::vector of \ref file.
        dependencies get_dependencies_recent() const;

        //! Executes commands for this \ref makefile_record.
        //! \param only_print if true, commands are only outputted to stdout.
        //! \return return value of first command to fail, or 0 if all commands returned 0.
        int execute(bool only_print = false);

        //! Checks whether commands for this \ref makefile_record has already been executed.
        //! \return true if it was, false otherwise.
        bool is_built() const { return m_process_state != process_states::NOT_YET_PROCESSED && m_process_state != process_states::PROCESSING; }

        //! Appends new command for this \ref makefile_record.
        //! \param command command to be added.
        void add_command(std::string&& command) { m_commands.emplace_back(std::move(command)); }

        //! Sets inner process state to being processed.
        void set_being_processed() { m_process_state = process_states::PROCESSING; }

        //! Checks, whether this record is currently being processed.
        //! \return true if it is, false otherwise.
        bool is_being_processed() { return m_process_state == process_states::PROCESSING; }

        //! Manually sets current process state.
        //! \param state \ref process_states to set to.
        void set_process_state(process_states state) { m_process_state = state; }

    private:
        targets m_targets; //!< targets of this \ref makefile_record.
        dependencies m_dependencies; //!< dependencies of this \ref makefile_record.
        std::vector<std::string> m_commands; //!< commands of this \ref makefile_record.
        process_states m_process_state; //!< Represent state in which this record is.
};
