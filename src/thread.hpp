#pragma once
#include <tuple>
#include <functional>
#include <pthread.h>

extern size_t static_counter;

/**!
    \brief Wrapper around POSIX threads pthread.
    \sa my_thread<Return(Input...)>
*/
template<typename F>
class my_thread;

/**!
    \brief Wrapper around POSIX threads pthread. Does not support functions without return value \a <void(Input...)>
    @tparam Return return type of the function.
    @tparam Input input types of the function.
*/
template<typename Return, typename... Input>
class my_thread<Return(Input...)>
{
    private:
        template<int...> struct seq { }; // idea based on http://stackoverflow.com/a/7858971/3087601
        template<int N, int... S> struct gens : gens<N-1, N-1, S...> { };
        template<int... S> struct gens<0, S...> { using type = seq<S...>; };

        //! struct containing data required for execution.
        struct thread_data
        {
            //! Constructor allowing function and input to be forwarded.
            //! @tparam F Function type.
            //! \param _func function value.
            //! \param _input input values.
            template<typename F>
            thread_data(F&& _func, Input&&... _input) : func(std::forward<F>(_func)), input(std::forward<Input>(_input)...), m_ready(false) { }
            
            //! Deleted copy constructor
            thread_data(const thread_data&) = delete;
            //! Deleted copy assignment.
            thread_data& operator=(const thread_data&) = delete;

            //! Defaulted move constructor.
            thread_data(thread_data&&) = default;
            //! Defaulted move assignment.
            thread_data& operator=(thread_data&&) = default;

            //! Call wrapper to allow proper argument forwarding.
            template<int... S> Return call_wrapper(seq<S...>){ return func(std::forward<Input>(std::get<S>(input))...); }

            private:
                std::function<Return(Input...)> func; //!< Function to be called in new thread.
            public:
                std::tuple<Input...> input; //!< Input arguments to the function.
                Return ret; //!< Return value of the function.
                bool m_ready; //!< Signalizes, that the function has been completed.
        };


    public:
        //! Constructor allowing function and input to be forwarded.
        //! @tparam F Function type.
        //! \param func function value.
        //! \param input input values.
        template<typename F>
        my_thread(F&& func, Input&&... input) : m_data(std::forward<F>(func), std::forward<Input>(input)...), m_joinable(true)
        {
            pthread_create(&m_id, NULL, thread_func, &m_data);
        }

        //! Deleted copy constructor
        my_thread(const my_thread&) = delete;
        //! Deleted copy assignment.
        my_thread& operator=(const my_thread&) = delete;
        
        //! Defaulted move constructor.
        my_thread(my_thread&&) = default;
        //! Defaulted move assignment.
        my_thread& operator=(my_thread&&) = default;

        //! Checks, whether this thread is joinable, i.e. not yet joined.
        bool joinable() const { return m_joinable; }

        //! Checks, whether this thread can be joined instantly.
        bool is_ready() const { return m_data.is_ready; }

        //! Joins this thread and returns computed value.
        //! \return return value of function given to this thread.
        Return join()
        {
            pthread_join(m_id, NULL);
            m_joinable = false;
            return m_data.ret;
        }
        
        //! Destructor to cancel thread if neccessary.
        ~my_thread() { if (m_joinable) pthread_cancel(m_id); }

        //! Detaches this thread, leaving return value unobtainable.
        void detach() { pthread_detach(m_id); m_joinable = false; }

    private:
        //! Call wrapper to allow member functions to be passed.
        //! \param arg pointer to \ref thread_data structure.
        //! \return NULL.
        static void* thread_func(void* arg)
        {
            thread_data* data = static_cast<thread_data*>(arg);
            data->ret = data->call_wrapper(typename gens<sizeof...(Input)>::type());
            data->m_ready = true;
            --static_counter;
            return NULL;
        }

        thread_data m_data; //!< Data this thread got in constructor.
        pthread_t m_id; //!< POSIX pthread_t id.
        bool m_joinable; //!< Indicates whether is this thread joinable.
};
