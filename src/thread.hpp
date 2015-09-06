#pragma once
#include <tuple>
#include <functional>
#include <pthread.h>

extern size_t static_counter;

template<typename F>
class my_thread;

template<typename Return, typename... Input>
class my_thread<Return(Input...)>
{
    private:
        template<int...> struct seq { }; // idea based on http://stackoverflow.com/a/7858971/3087601
        template<int N, int... S> struct gens : gens<N-1, N-1, S...> { };
        template<int... S> struct gens<0, S...> { using type = seq<S...>; };

        struct thread_data
        {
            template<typename F>
            thread_data(F&& _func, Input&&... _input) : func(std::forward<F>(_func)), input(std::forward<Input>(_input)...), m_ready(false) { }

            thread_data(const thread_data&) = delete;
            thread_data& operator=(const thread_data&) = delete;

            thread_data(thread_data&&) = default;
            thread_data& operator=(thread_data&&) = default;

            template<int... S> Return call_wrapper(seq<S...>){ return func(std::forward<Input>(std::get<S>(input))...); }

            private:
                std::function<Return(Input...)> func;
            public:
                std::tuple<Input...> input;
                Return ret;
                bool m_ready;
        };


    public:
        template<typename F>
        my_thread(F&& f, Input&&... value) : m_data(std::forward<F>(f), std::forward<Input>(value)...), m_joinable(true)
        {
            pthread_create(&m_id, NULL, thread_func, &m_data);
        }

        my_thread(const my_thread&) = delete;
        my_thread& operator=(const my_thread&) = delete;
        
        my_thread(my_thread&&) = default;
        my_thread& operator=(my_thread&&) = default;

        bool joinable() const { return m_joinable; }
        bool is_ready() const { return m_data.is_ready; }

        Return join()
        {
            pthread_join(m_id, NULL);
            m_joinable = false;
            return m_data.ret;
        }
        ~my_thread() { pthread_cancel(m_id); }

        void detach() { pthread_detach(m_id); m_joinable = false; }

    private:
        static void* thread_func(void* arg)
        {
            thread_data* data = static_cast<thread_data*>(arg);
            data->ret = data->call_wrapper(typename gens<sizeof...(Input)>::type());
            data->m_ready = true;
            --static_counter;
            return NULL;
        }

        thread_data m_data;
        pthread_t m_id;
        bool m_joinable;
};
