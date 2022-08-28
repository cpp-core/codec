// Copyright 2018, 2019, 2021, 2022 by Mark Melton
//

#include <filesystem>
#include <gtest/gtest.h>
#include "core/codex/zstd/compress.h"
#include "core/codex/zstd/compressor.h"
#include "core/codex/zstd/decompress.h"
#include "core/codex/zstd/decompress_to.h"
#include "core/codex/zstd/decompressor.h"
#include "core/codex/zstd/file_compressor.h"
#include "core/codex/zstd/file_decompressor.h"
#include "core/codex/zstd/zstd_fstream.h"
#include "core/concurrent/scoped_task.h"
#include "core/concurrent/queue/lockfree_spsc.h"
#include "core/concurrent/queue/sink_spsc.h"
#include "core/concurrent/queue/source_spsc.h"
#include "coro/stream/stream.h"

static const size_t NumberSamples = 32;

using namespace coro;
namespace fs = std::filesystem;

class Environment : public ::testing::Environment {
public:
    Environment()
	: root_(fs::temp_directory_path()) {
	root_ += fmt::format("/path.{}", getpid());
    }
    ~Environment() override { }
    void SetUp() override { fs::create_directories(root_); }
    void TearDown() override { fs::remove_all(root_); }

    string tmpfile() {
	++counter_;
	return fmt::format("{}/{}", root_, counter_);
    }
    
private:
    size_t counter_{0};
    string root_;
	
};

Environment *env{nullptr};

TEST(Zstd, Basic)
{
    auto g = str::alpha();
    for (auto str : take(std::move(g), NumberSamples / 4)) {
	auto file = env->tmpfile();
	zstd::Compressor c{std::ofstream{file}, 64};

	for (auto i = 0ul; i < str.size(); i += 1024) {
	    auto edx = std::min(i + 1024, str.size());
	    c.write(&str[i], &str[edx]);
	}
	c.close();

	zstd::Decompressor d{std::ifstream{file}, 64};
	string ustr;
	while (d.underflow())
	    ustr += d.view();
	
	EXPECT_EQ(str, ustr);
    }
}

TEST(Zstd, Pods)
{
    std::stringstream ss;
    zstd::Compressor c{ss, 2};

    int a = -42;
    int64 b = -37;
    float x = -1.0;
    double y = -2.0;
    c.write_pod(a);
    c.write_pod(b);
    c.write_pod(x);
    c.write_pod(y);
    c.close();

    zstd::Decompressor d{ss, 2};
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


TEST(Zstd, String)
{
    auto g = str::any(0, 1024);
    for (auto str : take(std::move(g), NumberSamples)) {
	auto zstr = zstd::compress(str);
	auto ustr = zstd::decompress(zstr);
	EXPECT_EQ(str, ustr);
    }
}

TEST(Zstd, Stream)
{
    auto g = str::any(0, 1024);
    for (auto str : take(std::move(g), NumberSamples)) {
	std::stringstream ss;
	
	core::mt::queue::SourceSpSc<char> source(str);
	zstd::compress(source, (std::ostream&)ss);
	
	core::mt::queue::SinkSpSc<char> sink;
	zstd::decompress((std::istream&)ss, sink);

	EXPECT_EQ(str, sink.data());
    }
}

TEST(Zstd, Queue)
{
    auto g = str::any(0, 1024);
    for (auto str : take(std::move(g), NumberSamples)) {
	core::mt::queue::SourceSpSc<char> source(str);
	core::mt::queue::SinkSpSc<char> sink;
	core::mt::queue::LockFreeSpSc<char> connector;

	auto task1 = core::mt::scoped_task([&]() { zstd::compress(source, connector); });
	auto task2 = core::mt::scoped_task([&]() { zstd::decompress(connector, sink); });
	task1.wait();
	task2.wait();
	EXPECT_EQ(str, sink.data());
    }
}

TEST(Zstd, Container)
{
    auto g = str::any(0, 1024);
    for (auto str : take(std::move(g), NumberSamples)) {
	core::mt::queue::SourceSpSc<char> source(str);
	core::mt::queue::LockFreeSpSc<char> connector;
	zstd::compress(source, connector);

	ints new_vec;
	zstd::decompress_to(connector, new_vec);
	
	// EXPECT_EQ(new_vec, str);
    }
}

TEST(Zstd, FileStream)
{
    const string file = env->tmpfile();
    {
	string line = "abc\n";
	core::zstd_ofstream zofs{file};
	zofs.write(line.data(), line.size());
    }
    {
	string line;
	core::zstd_ifstream zifs{file};
	auto r = (bool)std::getline(zifs, line);
	EXPECT_TRUE(r);
	EXPECT_EQ(line, "abc");
    }
}

TEST(Zstd, FileDecompressor)
{
    const string file = env->tmpfile();
    {
	string line = "abc\n";
	zstd::FileCompressor zofs{file};
	zofs.write(line.data(), line.size());
    }
    {
	string line;
	zstd::FileDecompressor zifs{file};
	auto r = zifs.read_line(line);
	EXPECT_TRUE(r);
	EXPECT_EQ(line, "abc");
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    env = new Environment{};
    AddGlobalTestEnvironment(env);
    return RUN_ALL_TESTS();
}



