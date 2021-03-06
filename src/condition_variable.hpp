#pragma once
#include <memory>
#include <pthread.h>
#include "mutex.hpp"

/**!
\brief Wrapper around pthread_cond.
*/
class condition_variable
{
    public:
        //! Default constructor.
        //! \param mutex Mutex to initialize \ref condition_variable with.
        condition_variable(mutex& mutex) : m_mutex(&mutex), m_cond(PTHREAD_COND_INITIALIZER) { }

        //! Defaulted move constructor.
        condition_variable(condition_variable&&) = default;

        //! Defaulted move assignment.
        condition_variable& operator=(condition_variable&&) = default;

        //! Deleted copy constructor.
        condition_variable(const condition_variable&) = delete;

        //! Deleted copy assignment.
        condition_variable& operator=(const condition_variable&) = delete;

        //! Broadcasts a signal on condition variable. Will unblock at least one thread.
        int signal() { return pthread_cond_signal(&m_cond); }

        //! Broadcasts a signal on condition variable. Will unblock all of threads.
        int broadcast() { return pthread_cond_broadcast(&m_cond); }

        //! Blocks itself until a signal is broadcast.
        int wait() { return pthread_cond_wait(&m_cond, &(m_mutex->m_mutex)); }

    private:
        mutex* m_mutex;
        pthread_cond_t m_cond;
};
