// Copyright 2018, 2019, 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "core/cc/queue/source_spsc.h"
#include "core/pp/seq.h"
#include "core/pp/map.h"
#include "core/pp/product.h"
#include "core/util/reinterpret_as.h"
#include "coro/stream/stream.h"

using ::testing::StaticAssertTypeEq;

static const size_t NumberSamples = 32;

TEST(SourceSpSc, Strings)
{
    for (auto str : coro::sampler<std::string>(0, 64) | coro::take(NumberSamples))
    {
	core::cc::queue::SourceSpSc source{str};
	std::string result;
	char ch;
	while (source.pop(ch))
	    result.push_back(ch);
	EXPECT_EQ(result, str);
    }
}

template<class T, class U>
void check()
{
    size_t count{0};
    for (auto vec : coro::sampler<std::vector<T>>(1, 64))
    {
	if ((sizeof(T) * vec.size()) % sizeof(U) != 0)
	    continue;
	    
	core::cc::queue::SourceSpSc<U> source{vec};
	std::vector<U> result;
	U value;
	while (source.pop(value))
	    result.push_back(value);
	
	EXPECT_EQ(reinterpret_as<T>(result), vec);

	++count;
	if (count >= NumberSamples)
	    break;
    }
}

#define CODE(A,B) check<A,B>();
#define CODE_SEQ(A) CODE(CORE_PP_HEAD_SEQ(A), CORE_PP_SECOND_SEQ(A))
#define TYPES() (std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t)
#define PRODUCT() CORE_PP_EVAL_CARTESIAN_PRODUCT_SEQ(TYPES(), TYPES())

TEST(SourceSpSc, TypeToType)
{
    CORE_PP_EVAL_MAP_SEQ(CODE_SEQ, PRODUCT())
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



