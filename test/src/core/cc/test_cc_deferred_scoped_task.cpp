// Copyright 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/cc/scoped_task.h"
#include "core/cc/deferred_scoped_task.h"

using core::cc::deferred_scoped_task;

TEST(DeferredScopedTask, Wait)
{
    {
	int a{0};
	deferred_scoped_task task{[&]() { a = 7; }};
	task.start();
	task.wait();
	EXPECT_EQ(a, 7);
    }

    {
	int a{0};
	deferred_scoped_task<void> task;
	task.assign([&]() { a = 7; });
	task.start();
	task.wait();
	EXPECT_EQ(a, 7);
    }
    
    {
	int a{0};
	deferred_scoped_task<void> task;
	task.start([&]() { a = 7; });
	task.wait();
	EXPECT_EQ(a, 7);
    }
}

TEST(DeferredScopedTask, WaitNoTask)
{
    deferred_scoped_task<void> task;
    task.wait();
    EXPECT_TRUE(true);
}

TEST(DeferredScopedTask, MultipleWaits)
{
    int a{0};
    deferred_scoped_task task{[&]() { a = 7; }};
    task.start();
    task.wait();
    task.wait();
    EXPECT_EQ(a, 7);
}

TEST(DeferredScopedTask, ReturnValue)
{
    {
	deferred_scoped_task task{[]() { return 2; }};
	task.start();
	EXPECT_EQ(task.get(), 2);
    }
    
    {
	deferred_scoped_task<int> task;
	task.assign([]() { return 2; });
	task.start();
	EXPECT_EQ(task.get(), 2);
    }
    
    {
	deferred_scoped_task<int> task;
	task.start([]() { return 2; });
	EXPECT_EQ(task.get(), 2);
    }
}

TEST(DeferredScopedTask, Exception)
{
    {
	deferred_scoped_task task{[]() { throw std::runtime_error("FooBar"); return 2; }};
	task.start();
	EXPECT_THROW(task.get(), std::runtime_error);
    }
    
    {
	deferred_scoped_task<int> task;
	task.assign([]() { throw std::runtime_error("FooBar"); return 2; });
	task.start();
	EXPECT_THROW(task.get(), std::runtime_error);
    }
    
    {
	deferred_scoped_task<int> task;
	task.start([]() { throw std::runtime_error("FooBar"); return 2; });
	EXPECT_THROW(task.get(), std::runtime_error);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



