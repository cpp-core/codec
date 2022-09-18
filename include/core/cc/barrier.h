// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <functional>
#include <mutex>

namespace core::cc
{

/// Downward counter that can be used to synchronize threads.
///
/// The counter is initialized on construction and is decremented as
/// threads arrive to the barrier. Once the counter reaches zero,
/// the supplied `completion` is run and strongly afterwards any
/// blocking threads are released and the counter is reset.
class Barrier {
public:
    using Functor = std::function<void()>;

    /// Construct a barrier.
    /// @param count The initial counter value
    /// @param completion The functor to run everytime the count reaches zero.
    explicit Barrier(std::ptrdiff_t count, Functor completion = nullptr);

    /// Destruct the barrier.
    ~Barrier() = default;

    /// Move only semantics.
    Barrier(const Barrier&) = delete;
    Barrier& operator=(Barrier&) = delete;

    /// Decrement the counter.
    void arrive();

    /// Block until the counter reaches zero
    void wait();

    /// Decrement the counter and block until the counter reaches zero
    /// and the completion is run.
    void arrive_and_wait();

    /// Decrement the counter and reduce the initial counter value.
    void arrive_and_drop();
    
private:
    /// Decrement the count. If count reaches zero, call the
    /// completion, reset the count, increment with generation, notify
    /// blocked threads and return false; otherwise, return
    /// true. Caller must have the mutex locked.
    bool with_mutex_decrement_count();
    
    /// Block on the current generation. Caller must have the mutex
    /// locked.
    void with_mutex_wait(std::unique_lock<std::mutex>& lock);
    
    std::ptrdiff_t threshold_, count_, generation_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::function<void()> completion_;
};

}; // core::cc
