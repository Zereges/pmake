#pragma once
#include <sys/stat.h>

class file
{
    public:
        file(std::string&& name, time_t t = 0) : m_name(move(name)), m_time(t)
        {
            struct stat buf;
            if (m_time == 0 && !stat(m_name.c_str(), &buf))
                m_time = buf.st_mtime;
        }

        time_t get_time() const { return m_time; }
        bool exists_on_disk() const { return m_time != 0; }
        const std::string& get_name() const { return m_name; }

        bool is_recent(const file& f) const { return get_time() > f.get_time(); }
        
        friend bool operator<(const file& t1, const file& t2) { return t1.m_name < t2.m_name; }
        friend bool operator>(const file& t1, const file& t2) { return t1.m_name > t2.m_name; }
        friend bool operator==(const file& t1, const file& t2) { return t1.m_name == t2.m_name; }
        friend bool operator!=(const file& t1, const file& t2) { return t1.m_name != t2.m_name; }

    private:
        std::string m_name;
        time_t m_time;
};
