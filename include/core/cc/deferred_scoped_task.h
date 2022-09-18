// Copyright (C) 2018, 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <future>

namespace core::cc
{

/// Create a new thread to start executing the given functor when
/// start is invoked.  Store the task result including any possible
/// exceptions in a `std::future`.
///
template<class T>
class deferred_scoped_task
{
public:
    template<class F>
    deferred_scoped_task(F&& function) {
	assign(std::forward<F>(function));
    }

    deferred_scoped_task() {
    }

    ~deferred_scoped_task() {
	if (m_thread.joinable())
	    m_thread.join();
    }

    void start() {
	m_thread = std::thread{std::move(m_task)};
    }

    template<class F>
    void start(F&& function) {
	assign(std::forward<F>(function));
	start();
    }

    deferred_scoped_task(deferred_scoped_task&& other) = default;
    deferred_scoped_task& operator=(deferred_scoped_task&& other) = default;
    
    deferred_scoped_task(const deferred_scoped_task&) = delete;
    deferred_scoped_task& operator=(const deferred_scoped_task&) = delete;

    template<class F>
    void assign(F&& function) {
	m_task = std::packaged_task<T()>(function);
	m_future = m_task.get_future();
    }

    T get() {
	if (m_future.valid())
	    return m_future.get();
	throw std::runtime_error("attempt to get invalid future");
    }
    
    void wait() const {
	if (m_future.valid())
	    m_future.wait();
    }
    
    auto& future() { return m_future; }
    const auto& future() const { return m_future; }
    
private:
    std::packaged_task<T()> m_task;
    std::future<T> m_future;
    std::thread m_thread;
};

template<class F> deferred_scoped_task(F&&) -> deferred_scoped_task<std::invoke_result_t<F>>;

}; // core::cc
