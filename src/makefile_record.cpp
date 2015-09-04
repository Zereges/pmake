#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <ctime>
#include "makefile_record.hpp"
#include "pmake_options.hpp"
#include "file.hpp"
#include "main.hpp"

makefile_record::makefile_record(std::vector<std::string>&& targets, std::vector<std::string>&& dependencies, const pmake_options& options) : m_completed(false)
{
    const std::vector<std::string> files = options.get_files();
    for (std::string& tar : targets)
        m_targets.emplace_back(std::move(tar), std::find(files.begin(), files.end(), tar) != files.end() ? time(0) : 0);
    for (std::string& dep : dependencies)
        m_dependencies.emplace_back(std::move(dep), std::find(files.begin(), files.end(), dep) != files.end() ? time(0) : 0);
}

bool makefile_record::is_recent(const file& tar) const
{
    for (const file& dep : m_dependencies)
        if (dep.is_recent(tar))
            return false;
    return true;
}

makefile_record::dependencies makefile_record::get_dependencies_stripped() const
{
    dependencies copy = m_dependencies;
    std::sort(copy.begin(), copy.end());
    copy.erase(std::unique(copy.begin(), copy.end()), copy.end());
    return std::move(copy);
}

makefile_record::dependencies makefile_record::get_dependencies_recent() const
{
    dependencies recent;
    for (const file& dep : m_dependencies)
        if (dep.is_recent(m_targets.front()))
            recent.push_back(dep);

    return std::move(recent);
}

int makefile_record::execute(bool only_print)
{
    for (const std::string& command : m_commands)
    {
        std::cout << command << std::endl;
        if (!only_print)
            if (int ret = system(command.c_str()))
                return ret;
    }
    m_completed = true;
    return 0;
}

int makefile_record::static_execute(makefile_record* record, bool only_print, std::string&& text)
{
    if (int ret = record->execute(only_print))
    {
        std::cerr << text << "' failed. (" << ret << "). Stopping." << std::endl;
        std::exit(CODE_FAILURE);
    }
    return 0;
}