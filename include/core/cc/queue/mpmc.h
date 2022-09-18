// Copyright (C) 2017, 2018, 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

namespace core::cc::queue
{

template<typename T>
class MpMc
{
public:
    using value_type = T;
    
    void push(const T& element)
    {
	std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push_back(element);
	lock.unlock();
        m_condition_variable.notify_one();
    }

    void push_sentinel()
    {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_sentinel = m_queue.size();
        m_condition_variable.notify_all();
    }

    bool empty() const
    {
	std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    bool pop_nowait(T& element)
    {
	std::unique_lock<std::mutex> lock(m_mutex);
        if (m_queue.empty() or m_sentinel == 0)
            return false;

	--m_sentinel;
        element = m_queue.front();
        m_queue.pop_front();
        return true;
    }

    bool pop(T& element)
    {
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_sentinel == 0)
	    return false;
	
        while (m_queue.empty())
	{
            m_condition_variable.wait(lock);
	    if (m_sentinel == 0)
		return false;
	}

	--m_sentinel;
        element = m_queue.front();
        m_queue.pop_front();
	return true;
    }

private:
    std::deque<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition_variable;
    int m_sentinel = -1;
};

}; // end ns core::cc::queue
