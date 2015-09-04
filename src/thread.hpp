#pragma once
#include <tuple>
#include <functional>
#include <pthread.h>

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
            thread_data(F&& _func, Input&&... _input) : func(std::move(_func)), input(std::move(_input)...) { }

            template<int... S> Return call_wrapper(seq<S...>) { return func(std::get<S>(std::move(input))...); }

            private:
                std::function<Return(Input...)> func;
            public:
                std::tuple<typename std::remove_reference<Input>::type...> input;
                Return ret;
        };


    public:
        template<typename F>
        my_thread(F&& f, Input&&... value) : m_data(std::forward<F>(f), std::forward<Input>(value)...), m_joinable(true)
        {
            pthread_create(&m_id, NULL, thread_func, &m_data);
        }

        bool joinable() const { return m_joinable; }

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
            return NULL;
        }

        thread_data m_data;
        pthread_t m_id;
        bool m_joinable;
};
