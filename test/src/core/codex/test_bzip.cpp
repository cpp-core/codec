// Copyright 2018, 2019, 2021 by Mark Melton
//

#include <gtest/gtest.h>
#include <range/v3/view/take.hpp>
// #include "core/codex/zstd/compressor.h"
// #include "core/codex/zstd/decompressor.h"
// #include "core/codex/zstd/compress.h"
// #include "core/codex/zstd/decompress.h"
// #include "core/codex/zstd/decompress_to.h"
#include "core/concurrent/scoped_task.h"
#include "core/concurrent/queue/lockfree_spsc.h"
#include "core/concurrent/queue/sink_spsc.h"
#include "core/concurrent/queue/source_spsc.h"
#include "core/range/sample.h"
#include "core/range/string.h"

static const size_t NumberSamples = 32;

TEST(ZSTD, Compressor)
{
    auto gsize = cr::uniform(66000, 67000);
    auto generator = cr::str::alpha(gsize);
    for (auto str : generator | v::take(1)) {
	std::stringstream ss;
	zstd::Compressor c{ss};

	for (auto i = 0ul; i < str.size(); i += 1024) {
	    auto edx = std::min(i + 1024, str.size());
	    c.write(&str[i], &str[edx]);
	}
	c.close();

	zstd::Decompressor d{ss};
	string ustr;
	while (d.underflow())
	    ustr += string_view{d.get().ptr_base(), d.get().position()};
	
	EXPECT_EQ(str, ustr);
    }
}

// TEST(ZSTD, String)
// {
//     auto gsize = cr::uniform(0, 1024);
//     auto generator = cr::str::any(gsize);
//     for (auto str : generator | v::take(NumberSamples))
//     {
// 	auto zstr = zstd::compress(str);
// 	auto ustr = zstd::decompress(zstr);
// 	EXPECT_EQ(str, ustr);
//     }
// }

// TEST(ZSTD, Stream)
// {
//     auto gsize = cr::uniform(0, 1024);
//     auto generator = cr::str::any(gsize);
//     for (auto str : generator | v::take(NumberSamples))
//     {
// 	std::stringstream ss;
	
// 	core::mt::queue::SourceSpSc<char> source(str);
// 	zstd::compress(source, (std::ostream&)ss);
	
// 	core::mt::queue::SinkSpSc<char> sink;
// 	zstd::decompress((std::istream&)ss, sink);

// 	EXPECT_EQ(str, sink.data());
//     }
// }

// TEST(ZSTD, Queue)
// {
//     auto gsize = cr::uniform(0, 1024);
//     auto generator = cr::str::any(gsize);
//     for (auto str : generator | v::take(NumberSamples))
//     {
// 	core::mt::queue::SourceSpSc<char> source(str);
// 	core::mt::queue::SinkSpSc<char> sink;
// 	core::mt::queue::LockFreeSpSc<char> connector;

// 	auto task1 = core::mt::scoped_task([&]() { zstd::compress(source, connector); });
// 	auto task2 = core::mt::scoped_task([&]() { zstd::decompress(connector, sink); });
// 	task1.wait();
// 	task2.wait();
// 	EXPECT_EQ(str, sink.data());
//     }
// }

// TEST(ZSTD, Container)
// {
//     auto gsize = cr::uniform(0, 1024);
//     auto generator = cr::str::any(gsize);
//     for (auto str : generator | v::take(NumberSamples))
//     {
// 	core::mt::queue::SourceSpSc<char> source(str);
// 	core::mt::queue::LockFreeSpSc<char> connector;
// 	zstd::compress(source, connector);

// 	ints new_vec;
// 	zstd::decompress_to(connector, new_vec);
	
// 	// EXPECT_EQ(new_vec, str);
//     }
// }

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



