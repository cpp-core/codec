// Copyright 2018, 2019, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/cc/queue/lockfree_spsc.h"
#include "core/cc/scoped_task.h"
#include "coro/stream/stream.h"

using ::testing::StaticAssertTypeEq;

template<class T>
void consumer(T& queue, const std::vector<int>& data)
{
    int i = 0, n;
    while (queue.pop(n))
    {
	EXPECT_EQ(n, data[i]);
	++i;
    }
}

TEST(LockFreeSpSc, SingleThread)
{
    core::cc::queue::LockFreeSpSc<int> queue{64, 8};
    for (auto i = 0; i < 32; ++i)
	queue.push(i);
    queue.push_sentinel();
    
    int count{0}, data{0};
    while (queue.pop(data))
    {
	EXPECT_LT(count, 32);
	EXPECT_EQ(data, count);
	++count;
    }
    EXPECT_EQ(count, 32);
}

TEST(LockFreeSpSc, MultiThreaded)
{
    const auto NumberExamples = 32;
    auto g = coro::sampler<size_t>(0, 2048)
	* coro::sampler<int>()
	| coro::sampler_vector();
    for (auto data : std::move(g) | coro::take(NumberExamples)) {
	core::cc::queue::LockFreeSpSc<int> queue{8, 4};
	std::thread sink([&]() { consumer(queue, data); });
    	for (size_t i = 0; i < data.size(); ++i)
    	    queue.push(data[i]);
    	queue.push_sentinel();
	sink.join();
    }
}

TEST(LockFreeSpSc, Throughput)
{
    for (auto i = 0; i < 1; ++i)
    {
	std::string input(1'000, ' ');
	std::string output(1'000, '.');
	core::cc::queue::LockFreeSpSc<char> queue{8, 4};
	core::cc::scoped_task producer([&]() { queue.push(input); queue.push_sentinel(); });
	auto ptr = output.data();
	auto end = ptr + output.size();
	size_t count{0};
	while (ptr < end and queue.pop(ptr, end, count))
	    ptr += count;
	producer.wait();
	EXPECT_EQ(input, output);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



