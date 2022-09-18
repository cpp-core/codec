// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <mutex>

namespace core::cc
{

class Gate {
public:
    // Construct an initially closed gate.
    Gate();

    // Set the gate to open.
    void open();

    // Set the gate to open and block until the gate is closed again.
    void open_and_wait();

    // Block until the gate is open.
    void wait_until_open();

    // Set the gate to closed.
    void close();

    // Set the gate to closed and block unitl the gate is open again.
    void close_and_wait();

    // Block until the gate is closed.
    void wait_until_closed();
    
private:
    bool state_;
    std::mutex open_mutex_, closed_mutex_;
    std::condition_variable open_cv_, closed_cv_;
};

}; // core::cc
