// Copyright 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "core/cc/barrier.h"
#include "core/cc/latch.h"

const int NumberThreads = 32;
const int NumberSyncs = 100;

TEST(ConcurrentBarrier, Example0)
{
    int sync_counter{0};
    std::atomic<int> thread_counter{0};
    
    core::cc::Barrier b{NumberThreads, [&]() {
	++sync_counter;
	EXPECT_EQ(thread_counter, NumberThreads);
	thread_counter = 0;
    }};
    core::cc::Latch l{NumberThreads};
    
    std::vector<std::thread> threads;
    for (auto i = 0; i < NumberThreads; ++i)
	threads.emplace_back([&]() {
	    for (auto j = 0; j < NumberSyncs; ++j) {
		thread_counter.fetch_add(1);
		b.arrive_and_wait();
	    }
	    l.arrive_and_wait();
	});

    l.wait();
    EXPECT_EQ(sync_counter, NumberSyncs);
    
    for (auto& thread : threads)
	if (thread.joinable())
	    thread.join();
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



