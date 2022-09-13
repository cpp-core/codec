// Copyright 2018, 2019, 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/codex/bzip/compress.h"
#include "core/codex/bzip/compressor.h"
#include "core/codex/bzip/decompress.h"
#include "core/codex/bzip/decompressor.h"
#include "core/cc/scoped_task.h"
#include "core/cc/queue/lockfree_spsc.h"
#include "core/cc/queue/sink_spsc.h"
#include "core/cc/queue/source_spsc.h"
#include "coro/stream/stream.h"

static const size_t NumberSamples = 1;

TEST(Bzip, Basic)
{
    for (auto str : coro::str::alpha(0, 4096) | coro::take(NumberSamples)) {
	std::stringstream ss;
	bzip::Compressor c{ss, 64};
	
	for (auto i = 0ul; i < str.size(); i += 1024) {
	    auto edx = std::min(i + 1024, str.size());
	    c.write(&str[i], edx - i);
	}
	c.close();

	bzip::Decompressor d{ss, 64};
	string ustr;
	while (d.underflow())
	    ustr += d.view();
	
	EXPECT_EQ(str, ustr);
    }
}

TEST(Bzip, Pods)
{
    std::stringstream ss;
    bzip::Compressor c{ss, 2};

    int a = -42;
    int64 b = -37;
    float x = -1.0;
    double y = -2.0;
    c.write_pod(a);
    c.write_pod(b);
    c.write_pod(x);
    c.write_pod(y);
    c.close();

    bzip::Decompressor d{ss, 2};
    int a2;
    int64 b2;
    float x2;
    double y2;
    
    EXPECT_TRUE(d.read_pod(a2));
    EXPECT_EQ(a2, a);
    
    EXPECT_TRUE(d.read_pod(b2));
    EXPECT_EQ(b2, b);
    
    EXPECT_TRUE(d.read_pod(x2));
    EXPECT_EQ(x2, x);
    
    EXPECT_TRUE(d.read_pod(y2));
    EXPECT_EQ(y2, y);
}


TEST(Bzip, Strings)
{
    for (auto str : coro::sampler<string>(0, 1024) | coro::take(NumberSamples)) {
	auto zstr = bzip::compress(str);
	auto ustr = bzip::decompress(zstr);
	EXPECT_EQ(str, ustr);
    }
}

// TEST(ZSTD, Stream)
// {
//     auto gsize = cr::uniform(0, 1024);
//     auto generator = cr::str::any(gsize);
//     for (auto str : generator | v::take(NumberSamples))
//     {
// 	std::stringstream ss;
	
// 	core::cc::queue::SourceSpSc<char> source(str);
// 	zstd::compress(source, (std::ostream&)ss);
	
// 	core::cc::queue::SinkSpSc<char> sink;
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
// 	core::cc::queue::SourceSpSc<char> source(str);
// 	core::cc::queue::SinkSpSc<char> sink;
// 	core::cc::queue::LockFreeSpSc<char> connector;

// 	auto task1 = core::cc::scoped_task([&]() { zstd::compress(source, connector); });
// 	auto task2 = core::cc::scoped_task([&]() { zstd::decompress(connector, sink); });
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
// 	core::cc::queue::SourceSpSc<char> source(str);
// 	core::cc::queue::LockFreeSpSc<char> connector;
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



