// Copyright (C) 2021, 2022 by Mark Melton
//

#include "core/cc/barrier.h"

namespace core::cc
{

Barrier::Barrier(std::ptrdiff_t count, Functor completion)
    : threshold_(count)
    , count_(count)
    , generation_(0)
    , completion_(std::move(completion)) {
}
    
void Barrier::arrive() {
    std::unique_lock lock{mutex_};
    with_mutex_decrement_count();
}
    
void Barrier::wait() {
    std::unique_lock lock{mutex_};
    with_mutex_wait(lock);
}

void Barrier::arrive_and_wait() {
    std::unique_lock lock{mutex_};
    if (with_mutex_decrement_count())
     	with_mutex_wait(lock);
}

bool Barrier::with_mutex_decrement_count() {
    if (--count_ != 0)
	return true;
    
    if (completion_)
	completion_();
    ++generation_;
    count_ = threshold_;
    cv_.notify_all();
    return false;
}

void Barrier::with_mutex_wait(std::unique_lock<std::mutex>& lock) {
    auto my_generation = generation_;
    cv_.wait(lock, [this,my_generation]() { return generation_ != my_generation; });
}
    
}; // core::cc

