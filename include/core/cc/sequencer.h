// Copyright (C) 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <mutex>
#include <condition_variable>

namespace core::cc
{

class Sequencer
{
public:
    Sequencer(size_t value = 0)
	: value_(value)
    { }

    void wait_for(size_t value)
    {
	std::unique_lock<std::mutex> lock{mutex_};
	while (value != value_)
	    cv_.wait(lock);
    }

    size_t value()
    {
	std::unique_lock<std::mutex> lock{mutex_};
	return value_;
    }

    void next(size_t n = 1)
    {
	std::unique_lock<std::mutex> lock{mutex_};
	value_ += n;
	cv_.notify_all();
    }

    void reset(size_t n = 0) {
	value_ = n;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    size_t value_;
};

}; // ns core::cc
