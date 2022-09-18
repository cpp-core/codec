// Copyright 2021, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include <thread>
#include "core/cc/ring/dependency_graph.h"
#include "core/cc/ring/claim.h"

namespace ring = core::cc::ring;

TEST(DependencyGraph, Simple)
{
    json j = {
	{ "replayer", {
		{ "size", 1024 },
		{ "write", { "sink" }}
	    }},
	{ "sink", {
		{ "size", 1024 },
		{ "read", { "replayer" }}
	    }}
    };

    ring::DependencyGraph graph;
    graph.configure(j);

    EXPECT_EQ(graph.size("replayer"), 1024);
    EXPECT_EQ(graph.size("sink"), 1024);

    ring::Processor<ring::SingleThreadClaimStrategy> a, b;
    graph.register_cursor(a.cursor(), "replayer");
    graph.register_cursor(b.cursor(), "sink");
    graph.wait_for_all_registrations();

    graph.apply_barriers(a, "replayer");
    graph.apply_barriers(b, "sink");

    EXPECT_EQ(a.read_barriers().size(), 0u);
    ASSERT_EQ(a.write_barriers().size(), 1u);
    EXPECT_EQ(a.write_barriers()[0], b.cursor());
    
    ASSERT_EQ(b.read_barriers().size(), 1u);
    EXPECT_EQ(b.write_barriers().size(), 0u);
    EXPECT_EQ(b.read_barriers()[0], a.cursor());
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



