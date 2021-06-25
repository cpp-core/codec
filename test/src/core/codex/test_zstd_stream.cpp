// Copyright 2018, 2019, 2021 by Mark Melton
//

#include <gtest/gtest.h>
#include <range/v3/view/take.hpp>
#include <sstream>
#include "core/codex/zstd/compress.h"
#include "core/codex/zstd/zstd_stream.h"
#include "core/range/sample.h"
#include "core/range/string.h"
#include "range/boolean.h"

static const int NumberSamples = 64;

TEST(Stream, Zstd)
{
    auto gsize = cr::uniform(0, 1024);
    auto generator = cr::str::alpha(gsize);
    for (auto str : generator | v::take(NumberSamples))
    {
	std::stringstream ss;
	{
	    core::zstd_ostream zout(ss, 32);
	    zout << str;
	}

	string result_str;
	core::zstd_istream zin(ss, 32);
	zin >> result_str;

	EXPECT_EQ(str, result_str);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



