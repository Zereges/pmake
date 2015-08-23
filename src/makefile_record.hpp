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

        makefile_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies, const pmake_options& options)
        {
            const std::vector<std::string> files = options.get_files();
            for (std::string& tar : targets)
                m_targets.emplace_back(std::move(tar), std::find(files.begin(), files.end(), tar) != files.end() ? time(0) : 0);
            for (std::string& dep : dependencies)
                m_dependencies.emplace_back(std::move(dep), std::find(files.begin(), files.end(), dep) != files.end() ? time(0) : 0);
        }

        const std::string& get_target() const { return m_targets.front().get_name(); }
        const dependencies& get_dependencies() const { return m_dependencies; }
        dependencies get_dependencies_stripped() const
        {
            dependencies copy = m_dependencies;
            std::sort(copy.begin(), copy.end());
            copy.erase(std::unique(copy.begin(), copy.end()), copy.end());
            return std::move(copy);
        }
        dependencies get_dependencies_recent() const
        {
            dependencies recent;
            std::for_each(m_dependencies.begin(), m_dependencies.end(), [&recent, this](const file& dep)
            {
                if (dep.is_recent(m_targets.front()))
                    recent.push_back(dep);
            });
            return std::move(recent);
        }
        void add_command(std::string&& command) { m_commands.emplace_back(std::move(command)); }
    private:
        targets m_targets;
        dependencies m_dependencies;
        std::vector<std::string> m_commands;
};
