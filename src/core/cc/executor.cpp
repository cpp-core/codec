// Copyright (C) 2021, 2022 by Mark Melton
//

#include <iostream>
#include "core/cc/executor.h"

namespace core::cc
{

template<class T>
ExecutorBase<T>::~ExecutorBase() {
    terminate();
}

template<class T>
void ExecutorBase<T>::wait() {
    std::unique_lock lock{set_mutex_};
    set_cv_.wait(lock, [this]{ return waiting_.empty() and active_.empty(); });
}

template<class T>
void ExecutorBase<T>::wait_for(size_t id) {
    std::unique_lock lock{set_mutex_};
    set_cv_.wait(lock, [this,id]{ return complete_.contains(id); });
}

template<class T>
void ExecutorBase<T>::terminate() {
    done_ = true;
    queue_.push_sentinel();
    for (auto& thread : pool_)
	thread.join();
    pool_.clear();
}

template<class T>
void ExecutorBase<T>::to_waiting(TaskId id) {
    std::lock_guard guard{set_mutex_};
    waiting_.insert(id);
    set_cv_.notify_all();
}

template<class T>
void ExecutorBase<T>::waiting_to_active(size_t id) {
    std::lock_guard guard{set_mutex_};
    waiting_.erase(id);
    active_.insert(id);
    set_cv_.notify_all();
}

template<class T>
void ExecutorBase<T>::active_to_complete(size_t id) {
    std::lock_guard guard{set_mutex_};
    active_.erase(id);
    complete_.insert(id);
    set_cv_.notify_all();
}

template class ExecutorBase<ExecutorDatum>;
template class ExecutorBase<ExecutorDatumCompletion>;

Executor::Executor(int number_threads) {
    for (auto i = 0; i < number_threads; ++i)
	pool_.emplace_back(std::thread{[this] () { loop(); }});
}

Executor::~Executor() {
    terminate();
}

std::size_t Executor::submit(Task task) {
    to_waiting(id_);
    queue_.push({id_, task});
    return id_++;
}

void Executor::loop() {
    Datum datum;
    while (not done_ and queue_.pop(datum)) {
	auto& [id, task] = datum;
	waiting_to_active(id);
	try { task(); }
	catch (std::exception& e) { std::cerr << e.what() << std::endl; }
	active_to_complete(id);
    }
}

ExecutorUnordered::ExecutorUnordered(int number_threads) {
    for (auto i = 0; i < number_threads; ++i)
	pool_.emplace_back(std::thread{[this] () { loop(); }});
}

size_t ExecutorUnordered::submit(Task task, Task completion) {
    to_waiting(id_);
    queue_.push({id_, task, completion});
    return id_++;
}

void ExecutorUnordered::loop() {
    Datum datum;
    while (not done_ and queue_.pop(datum)) {
	auto& [id, task, completion] = datum;
	waiting_to_active(id);
	try {
	    task();
	    std::unique_lock<std::mutex> lock{completion_mutex_};
	    completion();
	}
	catch (std::exception& e) {
	    std::cerr << e.what() << std::endl;
	}
	active_to_complete(id);
    }
}

ExecutorOrdered::ExecutorOrdered(int number_threads) {
    for (auto i = 0; i < number_threads; ++i)
	pool_.emplace_back(std::thread{[this] () { loop(); }});
}

size_t ExecutorOrdered::submit(Task task, Task completion) {
    to_waiting(id_);
    queue_.push({id_, task, completion});
    return id_++;
}

void ExecutorOrdered::loop() {
    Datum datum;
    while (not done_ and queue_.pop(datum)) {
	auto& [id, task, completion] = datum;
	waiting_to_active(id);
	try {
	    task();
	    sequencer_.wait_for(id);
	    completion();
	    sequencer_.next();
	}
	catch (std::exception& e) {
	    std::cerr << e.what() << std::endl;
	}
	active_to_complete(id);
    }
}

}; // core::cc
