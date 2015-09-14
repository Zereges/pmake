#pragma once
#include <pthread.h>

/**!
    \brief Wrapper around pthread_mutex.
*/
class mutex
{
    public:
        //! Basic constructor for \ref mutex.
        mutex() : m_mutex(PTHREAD_MUTEX_INITIALIZER) { }

        //! Defaulted move constructor.
        mutex(mutex&&) = default;

        //! Defaulted move assignment.
        mutex& operator=(mutex&&) = default;

        //! Deleted copy constructor.
        mutex(const mutex&) = delete;

        //! Deleted copy assignment.
        mutex& operator=(const mutex&) = delete;

        //! Destructor for \ref mutex, which frees inner structures.
        ~mutex() { pthread_mutex_destroy(&m_mutex); }

        //! Locks the mutex. Will block if mutex is already locked.
        //! \return true on success, false otherwise.
        bool lock() { return !pthread_mutex_lock(&m_mutex); }

        //! Tries to lock the mutex. If mutex is already locked, function returns immediately.
        //! \return true if mutex was locked by the call, false if mutex is already locked.
        bool try_lock() { return !pthread_mutex_trylock(&m_mutex); }

        //! Unlocks the mutex.
        //! \return true if unlock was successful, false if thread does not own the mutex.
        bool unlock() { return !pthread_mutex_unlock(&m_mutex); }

    private:
        pthread_mutex_t m_mutex; //!< pthread_mutex structure.

        friend class condition_variable;
};
