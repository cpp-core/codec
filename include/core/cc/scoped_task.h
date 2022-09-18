// Copyright (C) 2018, 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <future>

namespace core::cc
{

/// Create a new thread to start executing the given functor on
/// construction. Store the task result including any possible
/// exceptions in a `std::future`.
///
template<class T>
class scoped_task
{
public:
    /// Create a new thread to start executing the given functor
    /// capturing the result in a `std::future`.
    ///
    template<class F>
    scoped_task(F&& function)
	: m_task(std::move(function))
	, m_future(m_task.get_future())
	, m_thread(std::move(m_task))
    { }

    ~scoped_task()
    {
	if (m_thread.joinable())
	    m_thread.join();
    }

    scoped_task(scoped_task&& other) = default;
    scoped_task& operator=(scoped_task&& other) = default;
    
    scoped_task(const scoped_task&) = delete;
    scoped_task& operator=(const scoped_task&) = delete;

    /// Return a copy of the `std::future` capturing the functor result.
    T get() { return m_future.get(); }

    /// Block until the functor completes.
    void wait() const { return m_future.wait(); }

    /// Get a reference to the `std::future` that captures the functor result.
    auto& future() { return m_future; }
    
    /// Get a const reference to the `std::future` that captures the functor result.
    const auto& future() const { return m_future; }
    
private:
    std::packaged_task<T()> m_task;
    std::future<T> m_future;
    std::thread m_thread;
};

template<class F> scoped_task(F&&) -> scoped_task<std::invoke_result_t<F>>;

}; // core::cc
