// Copyright 2021, 2022 by Mark Melton
//

#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include "core/cc/executor.h"

using namespace std::chrono_literals;

TEST(ConcurrentExecutor, ConcurrentTasks)
{
    int counter{0};
    int number_tasks{64};
    core::cc::Executor ex;
    for (auto i = 0; i < number_tasks; ++i)
	ex.submit([&]{
	    auto n = counter;
	    std::this_thread::sleep_for(1ms);
	    counter = n + 1;
	});
    ex.wait();
    EXPECT_NE(counter, number_tasks);
}

TEST(ConcurrentExecutor, SequentialCompletions)
{
    bool active{false};
    core::cc::ExecutorOrdered ex;
    for (auto i = 0; i < 64; ++i)
	ex.submit([]{ std::this_thread::sleep_for(1ms); },
		  [&] {
		      EXPECT_FALSE(active);
		      active = true;
		      std::this_thread::sleep_for(1ms);
		      EXPECT_TRUE(active);
		      active = false;
		  });
    ex.wait();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



