// Copyright 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "core/cc/gate.h"
#include "core/cc/barrier.h"

const int NumberThreads = 16;
const int NumberLoops = 16;

TEST(ConcurrentGate, Example0)
{
    std::vector<int> pos;
    core::cc::Gate gate;
    core::cc::Barrier barrier{NumberThreads, [&](){ gate.close(); } };

    std::vector<std::thread> threads;
    for (auto i = 0; i < NumberThreads; ++i) {
	pos.push_back(0);
	threads.emplace_back([&,i]() {
	    for (auto j = 0; j < NumberLoops; ++j) {
		gate.wait_until_open();
		pos[i] = j;
		barrier.arrive_and_wait();
	    }
	});
    }

    for (auto i = 0; i < NumberLoops; ++i) {
	gate.open();
	gate.wait_until_closed();
	for (auto j = 0; j < threads.size(); ++j)
	    EXPECT_EQ(pos[j], i);
    }

    for (auto& thread : threads)
	if (thread.joinable())
	    thread.join();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



