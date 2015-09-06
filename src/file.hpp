#pragma once
#include <sys/stat.h>

/**!
    \brief Class representing basic file having name and timestamp of last modification.
*/
class file
{
    public:
        //! Constructor initalizing m_name and m_time with proper values.
        //! \param name Name of the file.
        //! \param t if non zero, overrides real (system) last modification time.
        file(std::string&& name, time_t t = 0) : m_name(move(name)), m_time(t)
        {
            struct stat buf;
            if (m_time == 0 && !stat(m_name.c_str(), &buf))
                m_time = buf.st_mtime;
        }

        //! Gets time of last modification.
        //! \return time of last modification.
        time_t get_time() const { return m_time; }

        //! Checks whether file really exists on disk.
        //! \return true if file exists, false otherwise.
        bool exists_on_disk() const { return m_time != 0; }

        //! Gets name of the \ref file.
        //! \return const reference to file name.
        const std::string& get_name() const { return m_name; }

        //! Checks, whether this \ref file is more recent than argument.
        //! \param f to check against.
        //! \return true if this \ref file is more recent than f, false otherwise.
        bool is_recent(const file& f) const { return get_time() > f.get_time(); }
        
        // comaring operators by m_name
        friend bool operator<(const file& t1, const file& t2) { return t1.m_name < t2.m_name; }
        friend bool operator>(const file& t1, const file& t2) { return t1.m_name > t2.m_name; }
        friend bool operator==(const file& t1, const file& t2) { return t1.m_name == t2.m_name; }
        friend bool operator!=(const file& t1, const file& t2) { return t1.m_name != t2.m_name; }

    private:
        std::string m_name; //!< Name of the file.
        time_t m_time; //!< Timestamp of last modification.
};
