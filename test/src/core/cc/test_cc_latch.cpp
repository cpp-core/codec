// Copyright 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "core/cc/latch.h"

const int NumberThreads = 32;

TEST(ConcurrentLatch, Example0)
{
    std::atomic<int> counter{0};
    core::cc::Latch l{NumberThreads};
    std::vector<std::thread> threads;
    for (auto i = 0; i < NumberThreads; ++i)
	threads.emplace_back([&]() {
	    counter.fetch_add(1);
	    l.arrive_and_wait();
	    EXPECT_EQ(counter, NumberThreads);
	});
    l.wait();
    EXPECT_EQ(counter, NumberThreads);

    for (auto& thread : threads)
	if (thread.joinable())
	    thread.join();
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



