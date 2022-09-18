// Copyright (C) 2017, 2018, 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <mutex>
#include <condition_variable>

namespace core::cc
{

template<typename T>
class Sync
{
public:
    Sync(const T& value = T{})
	: m_value(value)
    { }

    void wait_for(const T& value)
    {
	std::unique_lock<std::mutex> lock{ m_mutex };
	while (value != m_value)
	    m_condition_variable.wait(lock);
    }

    T get_value(const T& value)
    {
	std::unique_lock<std::mutex> lock{ m_mutex };
	return m_value;
    }
    
    void set_value(const T& value)
    {
	std::unique_lock<std::mutex> lock{ m_mutex };
	m_value = value;
	m_condition_variable.notify_all();
    }
    
private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condition_variable;
    T m_value;
};

}; // core::cc
