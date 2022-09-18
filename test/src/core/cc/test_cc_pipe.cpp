// Copyright 2018, 2019, 2021, 2022 by Mark Melton
//

#include <atomic>
#include <gtest/gtest.h>
#include "core/cc/pipe.h"

TEST(Pipe, Basic)
{
    core::cc::pipe::istream is("echo uno");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(is, line))
	lines.push_back(line);

    EXPECT_EQ(lines.size(), 1);
    EXPECT_EQ(lines[0], "uno");
}

TEST(Pipe, MultipleThreadsFailure)
{
    const size_t NumberThreads = 10;
    const size_t NumberRuns = 25;
    
    std::atomic<size_t> failure_count{0};
    vector<std::thread> threads;
    for (size_t i = 0; i < NumberThreads; ++i)
	threads.emplace_back(std::thread([&]() {
					     for (size_t i = 0; i < NumberRuns; ++i)
					     {
						 core::cc::pipe::istream is("echo uno");
						 std::string line;
						 if (not std::getline(is, line))
						     ++failure_count;
					     }
					 }));

    for (auto& thread : threads)
	thread.join();

#ifdef __APPLE__
    EXPECT_GT(failure_count, 3);
#else
    EXPECT_EQ(failure_count, 0);
#endif
}

TEST(Pipe, MultipleThreadsSuccess)
{
    const size_t NumberThreads = 10;
    const size_t NumberRuns = 25;
    
    std::atomic<size_t> failure_count{0};
    vector<std::thread> threads;
    for (size_t i = 0; i < NumberThreads; ++i)
	threads.emplace_back(std::thread([&]() {
					     for (size_t i = 0; i < NumberRuns; ++i)
					     {
						 core::cc::pipe::istream is("echo uno", 5);
						 std::string line;
						 if (not std::getline(is, line))
						     ++failure_count;
					     }
					 }));

    for (auto& thread : threads)
	thread.join();

    EXPECT_EQ(failure_count, 0);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



