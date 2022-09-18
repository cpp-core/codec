// Copyright (C) 2021, 2022 by Mark Melton
//

#include "core/cc/latch.h"

namespace core::cc
{

Latch::Latch(std::ptrdiff_t count)
    : count_(count) {
}
    
void Latch::count_down(std::ptrdiff_t n) {
    std::unique_lock lock{mutex_};
    with_mutex_decrement_count(n);
}

bool Latch::try_wait() {
    std::unique_lock lock{mutex_};
    return count_ == 0;
}
    
void Latch::wait() {
    std::unique_lock lock{mutex_};
    with_mutex_wait(lock);
}
    
void Latch::arrive_and_wait() {
    std::unique_lock lock{mutex_};
    if (with_mutex_decrement_count(1))
	with_mutex_wait(lock);
}

bool Latch::with_mutex_decrement_count(std::ptrdiff_t n) {
    count_ -= n;
    if (count_ != 0)
	return true;
    cv_.notify_all();
    return false;
}

void Latch::with_mutex_wait(std::unique_lock<std::mutex>& lock) {
    cv_.wait(lock, [this]() { return count_ == 0; });
}

}; // core::cc

