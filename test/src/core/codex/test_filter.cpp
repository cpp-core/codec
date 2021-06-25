// Copyright 2018, 2019, 2021 by Mark Melton
//

#include <gtest/gtest.h>
#include <range/v3/view/take.hpp>
#include <sstream>
#include "core/codex/filter.h"
#include "core/range/sample.h"
#include "core/range/string.h"

static const int NumberSamples = 64;

TEST(Stream, Filter)
{
    const int MaxLineSize = 128;
    const int LineFilter = 64;
    
    for (auto i = 0; i < NumberSamples; ++i) {
	string lines;
	auto gsize = cr::uniform(0, MaxLineSize);
	auto generator = cr::str::alpha(gsize);

	uint expected_count{0};
	for (auto str : generator | v::take(64)) {
	    if (str.size() < LineFilter)
		++expected_count;
	    lines += str;
	    lines += '\n';
	}
	std::stringstream ss{lines};
	core::filter_istream fin(ss, [](string_view s) { return s.size() < LineFilter; });
	string line;
	uint count{0};
	while (std::getline(fin, line)) {
	    if (line.size() < LineFilter)
		++count;
	    EXPECT_LT(line.size(), LineFilter);
	}
	EXPECT_EQ(count, expected_count);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



