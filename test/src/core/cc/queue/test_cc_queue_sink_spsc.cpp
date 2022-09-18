// Copyright 2018, 2019, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/cc/queue/sink_spsc.h"
#include "core/pp/seq.h"
#include "core/pp/map.h"
#include "core/pp/product.h"
#include "coro/stream/stream.h"

static const size_t NumberSamples = 32;

TEST(SinkSpSc, Strings)
{
    for (auto str : coro::sampler<std::string>(0, 64) | coro::take(NumberSamples))
    {
	core::cc::queue::SinkSpSc<char> sink;
	for (auto ch : str)
	    sink.push(ch);
	EXPECT_EQ(sink.data(), str);
    }
}

template<class T>
void check()
{
    for (auto vec : coro::sampler<std::vector<T>>(1, 64) | coro::take(NumberSamples))
    {
	core::cc::queue::SinkSpSc<T> sink;
	for (auto elem : vec)
	    sink.push(elem);
	EXPECT_EQ(sink.data(), vec);
    }
}

TEST(SinkSpSc, TypeToType)
{
    check<std::int8_t>();
    check<std::int16_t>();
    check<std::int32_t>();
    check<std::int64_t>();
    check<std::uint8_t>();
    check<std::uint16_t>();
    check<std::uint32_t>();
    check<std::uint64_t>();
    check<float>();
    check<double>();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



