// Copyright 2018, 2019, 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include <thread>
#include "core/cc/sequencer.h"
#include "coro/stream/stream.h"

using ::testing::StaticAssertTypeEq;

void reduce(size_t idx, core::cc::Sequencer& gate, std::vector<size_t>& data)
{
    gate.wait_for(idx);
    data.push_back(idx);
    gate.next();
}

TEST(ConcurrentSequencer, Example0)
{
    const size_t number_threads = 32;
    core::cc::Sequencer gate;

    std::vector<size_t> data;
    std::vector<std::thread> threads;
    for (size_t idx = 0; idx < number_threads; ++idx)
	threads.emplace_back(std::thread([=,&gate,&data]() { reduce(idx, gate, data); }));

    for (auto& thread : threads)
	thread.join();

    for (auto [i, idx] : coro::iota<size_t>(0) * data | coro::zip())
	EXPECT_EQ(i, idx); 
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



