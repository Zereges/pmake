#pragma once
#include <vector>
#include <string>


class makefile_record
{
    
    private:
        std::vector<std::string> m_targets;
        std::vector<std::string> m_dependencies;
        std::vector<std::string> m_commands;
};