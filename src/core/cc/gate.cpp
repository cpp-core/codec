// Copyright (C) 2021, 2022 by Mark Melton
//

#include "core/cc/gate.h"

namespace core::cc
{

Gate::Gate()
    : state_(false) {
}

void Gate::open() {
    std::unique_lock lock{open_mutex_};
    state_ = true;
    open_cv_.notify_all();
}

void Gate::wait_until_open() {
    std::unique_lock lock{open_mutex_};
    open_cv_.wait(lock, [&]() { return state_; });
}

void Gate::open_and_wait() {
    std::unique_lock closed_lock{closed_mutex_};
    {
	std::unique_lock lock{open_mutex_};
	state_ = true;
	open_cv_.notify_all();
    }
    closed_cv_.wait(closed_lock, [&]() { return not state_; });
}
    
void Gate::close() {
    std::unique_lock lock{closed_mutex_};
    state_ = false;
    closed_cv_.notify_all();
}

void Gate::wait_until_closed() {
    std::unique_lock lock{closed_mutex_};
    closed_cv_.wait(lock, [&]() { return not state_; });
}

}; // core::cc

