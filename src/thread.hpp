#pragma once
#include <pthread.h>
#include <tuple>
#include <functional>

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
            thread_data(F&& _func, Input&&... _input) : input(std::forward<Input>(_input)...), func(std::move(_func)) { }

            template<int... S> Return call_wrapper(seq<S...>) { return func(std::get<S>(input)...); }

            std::tuple<Input...> input;
            Return ret;

            private:
                std::function<Return(const Input&...)> func;
        };


    public:
        template<typename F>
        my_thread(F&& f, Input&&... value) : m_data(std::move(f), std::forward<Input>(value)...), m_joinable(true)
        {
            pthread_create(&m_id, NULL, thread_func, &m_data);
        }

        Return join()
        {
            pthread_join(m_id, NULL);
            m_joinable = false;
            return m_data.ret;
        }
        ~my_thread() { if (m_joinable) pthread_join(m_id, NULL); }

        void detach() { pthread_detach(m_id); m_joinable = false; }

    private:
        static void* thread_func(void* arg)
        {
            thread_data* data = static_cast<thread_data*>(arg);
            data->ret = data->call_wrapper(typename gens<sizeof...(Input)>::type());
            return NULL;
        }

        std::function<Return(const Input&...)> m_func;
        thread_data m_data;
        pthread_t m_id;
        bool m_joinable;
};
