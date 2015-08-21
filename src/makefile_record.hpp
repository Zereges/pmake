#pragma once
#include <vector>
#include <string>


class makefile_record
{
    public:
        makefile_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies) :
            m_targets(std::move(targets)), m_dependencies(std::move(dependencies)) { }

        const std::string& get_target() const { return m_targets.front(); }
        const std::vector<std::string>& get_dependencies() const { return m_dependencies; }
        std::vector<std::string> get_dependencies_stripped() const
        {
            std::vector<std::string> copy = m_dependencies;
            std::sort(copy.begin(), copy.end());
            copy.erase(std::unique(copy.begin(), copy.end()), copy.end());
            return std::move(copy);
        }

        void add_command(std::string&& command) { m_commands.emplace_back(std::move(command)); }
    private:
        std::vector<std::string> m_targets;
        std::vector<std::string> m_dependencies;
        std::vector<std::string> m_commands;
};