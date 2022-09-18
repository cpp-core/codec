// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <mutex>

namespace core::cc
{

/// Downward counter that can be used to synchronize threads.
///
/// The counter is initialized on construction and can only be
/// decremented as threads arrive making Latch a single use Barrier.
///
/// A Latch has move semantics.
class Latch {
public:
    /// Construct a latch with the given count.
    /// @param count The initial counter value. 
    explicit Latch(std::ptrdiff_t count);

    // Destruct the latch
    ~Latch() = default;

    // Move only semantics.
    Latch(const Latch&) = delete;
    Latch& operator=(Latch&) = delete;

    /// Decrements the counter without blocking.
    void count_down(std::ptrdiff_t n = 1);

    /// Return true if the counter is zero; otherwise, false;
    bool try_wait();
    
    /// Blocks until the counter reaches zero.
    void wait();

    /// Decrements the counter and blocks until it reaches zero.
    void arrive_and_wait();

private:
    /// Decrement the count. If count reaches zero, notify block
    /// threads and return false; otherwise, return true. Caller must
    /// have the mutex locked.
    bool with_mutex_decrement_count(std::ptrdiff_t n);
    
    /// Block on the current generation. Caller must have the mutex
    /// locked.
    void with_mutex_wait(std::unique_lock<std::mutex>& lock);
    
    std::ptrdiff_t count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

}; // core::cc
