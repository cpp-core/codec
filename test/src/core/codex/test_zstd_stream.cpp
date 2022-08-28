// Copyright 2018, 2019, 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include <sstream>
#include "core/codex/zstd/compress.h"
#include "core/codex/zstd/zstd_stream.h"
#include "coro/stream/stream.h"

static const int NumberSamples = 64;

TEST(Zstd, Stream)
{
    for (auto str : coro::str::alpha(0, 1024) | coro::take(NumberSamples)) {
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



