#pragma once
#include <string>
#include <sys/stat.h>

class dependency
{
    public:
        dependency(std::string&& name, time_t t = 0) : m_name(move(name)), m_time(t)
        {
            struct stat buf;
            if (time == 0 && !stat(m_name.c_str(), &buf))
                m_time = buf.st_mtime;

        }

        time_t get_time() const { return m_time; }
        const std::string& get_name() const { return m_name; }
        
        friend bool operator<(const dependency& t1, const dependency& t2) { return t1.m_name < t2.m_name; }
        friend bool operator>(const dependency& t1, const dependency& t2) { return t1.m_name > t2.m_name; }
        friend bool operator==(const dependency& t1, const dependency& t2) { return t1.m_name == t2.m_name; }
        friend bool operator!=(const dependency& t1, const dependency& t2) { return t1.m_name != t2.m_name; }

    private:
        std::string m_name;
        time_t m_time;
};
