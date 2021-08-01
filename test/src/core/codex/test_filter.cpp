// Copyright 2018, 2019, 2021 by Mark Melton
//

#include <gtest/gtest.h>
#include <range/v3/view/take.hpp>
#include <sstream>
#include "core/codex/filter.h"
#include "core/codex/filter_comments.h"
#include "core/range/sample.h"
#include "core/range/string.h"

static const int NumberSamples = 64;

TEST(Codex, Filter)
{
    const int MaxLineSize = 128;
    const int LineFilter = 64;
    
    for (auto i = 0; i < NumberSamples; ++i) {
	string lines;
	auto gsize = cr::uniform(0, MaxLineSize);
	auto generator = cr::str::alpha(gsize);

	uint expected_count{0};
	for (auto str : generator | v::take(MaxLineSize)) {
	    str += '\n';
	    if (str.size() < LineFilter) {
		++expected_count;
	    }
	    lines += str;
	}
	
	std::stringstream ss{lines};
	core::filter_istream fin(ss, [](string_view s) { return s.size() < LineFilter; });
	
	string line;
	uint count{0};
	while (std::getline(fin, line)) {
	    ++count;
	    EXPECT_LT(line.size(), LineFilter);
	}
	EXPECT_EQ(count, expected_count);
    }
}

size_t count_lines(const string& str) {
    std::stringstream ss{str};
    auto sin = core::filter_comments(ss);
    
    string line;
    size_t count{0};
    while (std::getline(sin, line))
	++count;
    return count;
}

TEST(CodexFilter, CommentsNoNewline)
{
    auto str = R"(// Artifact properties test0
//
{
    "tsdb::Instrument" : "BTC/USD",
    "tsdb::Exchange" : "kraken",
    "tsdb::Organization" : "per-instrument",
    "tsdb::Source" : "next-prime"
})";

    EXPECT_EQ(count_lines(str), 6);
}

TEST(CodexFilter, CommentsNewline)
{
    auto str = R"(// Artifact properties test0
//
{
    "tsdb::Instrument" : "BTC/USD",
    "tsdb::Exchange" : "kraken",
    "tsdb::Organization" : "per-instrument",
    "tsdb::Source" : "next-prime"
}
)";

    EXPECT_EQ(count_lines(str), 6);
}

TEST(CodexFilter, CommentsCommentNoNewline)
{
    auto str = R"(// Artifact properties test0
//
{
    "tsdb::Instrument" : "BTC/USD",
    "tsdb::Exchange" : "kraken",
    "tsdb::Organization" : "per-instrument",
    "tsdb::Source" : "next-prime"
}
// end)";

    EXPECT_EQ(count_lines(str), 6);
}

TEST(CodexFilter, CommentsCommentNewline)
{
    auto str = R"(// Artifact properties test0
//
{
    "tsdb::Instrument" : "BTC/USD",
    "tsdb::Exchange" : "kraken",
    "tsdb::Organization" : "per-instrument",
    "tsdb::Source" : "next-prime"
}
// end
)";

    EXPECT_EQ(count_lines(str), 6);
}


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



