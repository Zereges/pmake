#pragma once
#include <vector>
#include <string>


class makefile_record
{
    public:
        makefile_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies) :
            m_targets(std::move(targets)), m_dependencies(std::move(dependencies)) { }

        void add_command(std::string&& command) { m_commands.emplace_back(std::move(command)); }
    private:
        std::vector<std::string> m_targets;
        std::vector<std::string> m_dependencies;
        std::vector<std::string> m_commands;
};