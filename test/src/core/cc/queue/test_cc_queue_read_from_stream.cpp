// Copyright 2018, 2019, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/cc/queue/read_from_stream.h"
#include "core/cc/queue/sink_spsc.h"
#include "coro/stream/stream.h"

constexpr size_t NumberSamples = 32;

TEST(ReadFromStream, Basic)
{
    auto g = coro::sampler<std::string>(0, 8 * 1024);
    for (auto str : std::move(g) | coro::take(NumberSamples))
    {
    	std::stringstream ss(str);
    	core::cc::queue::SinkSpSc<char> sink;
    	core::cc::queue::read_from_stream(ss, sink);
    	EXPECT_EQ(str, sink.data());
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



