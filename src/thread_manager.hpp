#pragma once
#include <list>
#include "pmake.hpp"

extern size_t static_counter;

/**!
    \brief Class to manage storing new threads.
*/
class thread_manager
{
    private:
        std::list<pmake::thread_type> m_threads; //!< list of \ref my_thread.

    public:
        //! Emplaces new thread by forwarding input values to \ref my_thread constructor.
        //! @tparam Ts types of input parameters.
        //! \param ts values of input parameters.
        template<typename... Ts>
        void emplace_back(Ts&&... ts)
        {
            m_threads.emplace_back(std::forward<Ts>(ts)...);
            ++static_counter;
        }

        //! Returns the iterator to begin of the container.
        //! \return iterator to the begin.
        decltype(m_threads)::iterator begin() { return m_threads.begin(); }

        //! Returns the iterator past the end of the container.
        //! \return iterator past the end.
        decltype(m_threads)::iterator end() { return m_threads.end(); }

        //! Returns the iterator to begin of the container.
        //! \return const_iterator to the begin.
        decltype(m_threads)::const_iterator begin() const { return m_threads.begin(); }

        //! Returns the iterator past the end of the container.
        //! \return const_iterator past the end.
        decltype(m_threads)::const_iterator end() const { return m_threads.end(); }

        //! Gets total threads in the program. Does not count threads which are waiting to be joined.
        //! \return total threads currently running in program.
        static size_t get_value() { return static_counter; }
};
