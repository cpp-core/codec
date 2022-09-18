// Copyright 2018, 2019, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/cc/queue/write_to_stream.h"
#include "core/cc/queue/source_spsc.h"
#include "coro/stream/stream.h"

constexpr size_t NumberSamples = 32;

TEST(WriteToStream, Basic)
{
    for (auto str : coro::sampler<std::string>(0, 8 * 1024) | coro::take(NumberSamples))
    {
	core::cc::queue::SourceSpSc<char> source(str);
	std::stringstream sink;
	core::cc::queue::write_to_stream(source, sink);
	EXPECT_EQ(str, sink.str());
    }
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



