// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <set>
#include <thread>
#include <vector>
#include "core/cc/queue/mpmc.h"
#include "core/cc/sequencer.h"

namespace core::cc
{

namespace detail {
using TaskId = size_t;
using Task = std::function<void()>;
};

// CRTP base class for Executor, ExecutorUnordered and ExecutorOrdered.
template<class T>
class ExecutorBase {
public:
    using TaskId = detail::TaskId;
    using Task = detail::Task;
    using Datum = T;
    using Queue = core::cc::queue::MpMc<Datum>;

    // Stop and join all execution threads.
    ~ExecutorBase();

    // Wait for all the waiting and active tasks to complete and then return.
    void wait();

    // Wait for the given task to complete and then return.
    void wait_for(size_t id);

    // Join all execution threads.
    void terminate();

protected:
    // Move the given task to the waiting set.
    void to_waiting(TaskId id);
    
    // Move the given task from waiting to active.
    void waiting_to_active(size_t id);

    // Move the given task from active to complete.
    void active_to_complete(size_t id);
    
    bool done_{false};
    size_t id_{0};
    std::set<size_t> waiting_, active_, complete_;
    std::mutex set_mutex_;
    std::condition_variable set_cv_;
    Queue queue_;
    std::vector<std::thread> pool_;
};

using ExecutorDatum = std::tuple<detail::TaskId, detail::Task>;
using ExecutorDatumCompletion = std::tuple<detail::TaskId, detail::Task, detail::Task>;

// Executor - Concurrent execution of independent tasks. Submitted
// tasks are executed asychronously on the underlying thread pool.
//
class Executor : public ExecutorBase<ExecutorDatum> {
public:
    // Construct an executor with the given number of execution
    // threads.
    Executor(int number_threads = std::thread::hardware_concurrency());

    // Stop and join all execution threads.
    ~Executor();

    // Submit the given task for execution and return the task
    // identifier.
    size_t submit(Task task);
    
private:
    // Internal task loop.
    void loop();
};

// ExecutorUnordered - Accepts pairs of task and completion
// functors. Submitted tasks are executed concurrently wrt to other
// tasks while completions are executed sequentially wrt other
// completions. This functionality is useful for MapReduce like
// operations where the reduce operation is unordered.
class ExecutorUnordered : public ExecutorBase<ExecutorDatumCompletion> {
public:
    // Construct an executor with the given number of execution
    // threads.
    ExecutorUnordered(int number_threads = std::thread::hardware_concurrency());

    // Submit the given task for execution and return the task
    // identifier.
    size_t submit(Task task, Task completion);
    
private:
    // Internal task loop.
    void loop();

    std::mutex completion_mutex_;
};

// ExecutorOrdered - Accepts pairs of task and completion
// functors. Submitted tasks are executed are executed concurrently
// wrt to other tasks, while the completions are executed sequentially
// in submission order wrt other completions. This functionality is
// useful for MapReduce like operations where the reduce operation is
// ordered.
//
class ExecutorOrdered : public ExecutorBase<ExecutorDatumCompletion> {
public:
    // Construct an executor with the given number of execution
    // threads.
    ExecutorOrdered(int number_threads = std::thread::hardware_concurrency());

    // Submit the given task and completion for execution and return
    // the task identifier.
    size_t submit(Task task, Task completion);

private:
    // Internal task loop.
    void loop();

    core::cc::Sequencer sequencer_;
};

}; // core::cc
