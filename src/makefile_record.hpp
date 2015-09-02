#pragma once
#include <vector>
#include <string>
#include "pmake_options.hpp"
#include "file.hpp"

class makefile_record
{
    public:
        using dependencies = std::vector<file>;
        using targets = std::vector<file>;

        makefile_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies, const pmake_options& options) : m_completed(false)
        {
            const std::vector<std::string> files = options.get_files();
            for (std::string& tar : targets)
                m_targets.emplace_back(std::move(tar), std::find(files.begin(), files.end(), tar) != files.end() ? time(0) : 0);
            for (std::string& dep : dependencies)
                m_dependencies.emplace_back(std::move(dep), std::find(files.begin(), files.end(), dep) != files.end() ? time(0) : 0);
        }

        bool is_recent(const file& tar) const
        {
            for (const file& dep : m_dependencies)
                if (dep.is_recent(tar))
                    return false;
            return true;
        }

        const file& get_target() const { return m_targets.front(); }
        const dependencies& get_dependencies() const { return m_dependencies; }
        bool has_target(const file& t) const { return find(m_targets.begin(), m_targets.end(), t) != m_targets.end(); }

        dependencies get_dependencies_stripped() const;
        dependencies get_dependencies_recent() const;

        int execute(bool only_print = false);
        bool is_built() const { return m_completed; }

        void add_command(std::string&& command) { m_commands.emplace_back(std::move(command)); }

    private:
        targets m_targets;
        dependencies m_dependencies;
        std::vector<std::string> m_commands;
        bool m_completed;
};
