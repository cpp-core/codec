// Copyright 2018, 2019, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/cc/scoped_task.h"

TEST(ScopedTask, Wait)
{
    int a{0};
    core::cc::scoped_task task{[&]() { a = 7; }};
    task.wait();
    EXPECT_EQ(a, 7);
}

TEST(ScopedTask, ReturnValue)
{
    core::cc::scoped_task task{[]() { return 2; }};
    EXPECT_EQ(task.get(), 2);
}

TEST(ScopedTask, Exception)
{
    core::cc::scoped_task task{[]() { throw std::runtime_error("FooBar"); return 2; }};
    EXPECT_THROW(task.get(), std::runtime_error);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



