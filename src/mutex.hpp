#pragma once
#include <pthread.h>

class mutex
{
    public:
        mutex() : m_mutex(PTHREAD_MUTEX_INITIALIZER) { }
        mutex(const mutex&) = delete;
        ~mutex() { pthread_mutex_destroy(&m_mutex); }

        bool lock() { return !pthread_mutex_lock(&m_mutex); }
        bool try_lock() { return !pthread_mutex_trylock(&m_mutex); }
        bool unlock() { return !pthread_mutex_unlock(&m_mutex); }

    private:
        pthread_mutex_t m_mutex;
};
