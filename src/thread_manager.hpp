#pragma once
#include <list>
#include "pmake.hpp"

extern size_t static_counter;

class thread_manager
{
    private:
        std::list<pmake::thread_type> m_threads;

    public:
        template<typename... Ts>
        void emplace_back(Ts&&... ts)
        {
            m_threads.emplace_back(std::forward<Ts>(ts)...);
            ++static_counter;
        }

        decltype(m_threads)::iterator begin() { return m_threads.begin(); }
        decltype(m_threads)::iterator end() { return m_threads.end(); }
        decltype(m_threads)::const_iterator begin() const { return m_threads.begin(); }
        decltype(m_threads)::const_iterator end() const { return m_threads.end(); }

        static size_t get_value() { return static_counter; }
};
