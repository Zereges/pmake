#pragma once
#include <vector>
#include <string>
#include "pmake_options.hpp"
#include "dependency.hpp"

class makefile_record
{
    public:
    typedef std::vector<dependency> dependencies;

        makefile_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies, const pmake_options& options) :
            m_targets(std::move(targets))
        {
            for (std::string& dep : dependencies)
            {
                const std::vector<std::string> files = options.get_files();
                m_dependencies.emplace_back(std::move(dep), std::find(files.begin(), files.end(), dep) != files.end() ? time(0) : 0);
            }
        }

        const std::string& get_target() const { return m_targets.front(); }
        const dependencies& get_dependencies() const { return m_dependencies; }
        dependencies get_dependencies_stripped() const
        {
            dependencies copy = m_dependencies;
            std::sort(copy.begin(), copy.end());
            copy.erase(std::unique(copy.begin(), copy.end()), copy.end());
            return std::move(copy);
        }

        void add_command(std::string&& command) { m_commands.emplace_back(std::move(command)); }
    private:
        std::vector<std::string> m_targets;
        dependencies m_dependencies;
        std::vector<std::string> m_commands;
};