// Copyright 2018, 2019, 2022 by Mark Melton
//

#include <gtest/gtest.h>
#include "core/cc/queue/lockfree_mpmc.h"
#include "core/cc/scoped_task.h"

TEST(ConcurrentQueueLockFreeMpMc, Fuzz)
{
    constexpr size_t number_threads = 32;
    constexpr size_t number_ops = 1000;

    core::cc::queue::LockFreeMpMc<size_t> queue;
    std::atomic<bool> start{false};

    std::vector<core::cc::scoped_task<void>> writers;
    std::vector<core::cc::scoped_task<size_t>> readers;
    for (size_t idx = 0; idx < number_threads; ++idx)
    {
	writers.emplace_back(core::cc::scoped_task<void>
			     ([&, idx]() {
				  while (not start);
				  for (auto i = idx; i < number_ops; i += number_threads)
				      queue.push(i);
			      }));
	readers.emplace_back(core::cc::scoped_task<size_t>
			     ([&, idx]() {
				  while (not start);
				  size_t local_sum{0};
				  for (auto i = idx; i < number_ops; i += number_threads)
				  {
				      size_t value{0};
				      queue.pop(value);
				      local_sum += value;
				  }
				  return local_sum;
			      }));
    }

    start = true;
    for (auto& task : writers)
	task.wait();
    // queue.push_sentinel();

    size_t count{0};
    for (auto& task : readers)
	count += task.get();

    EXPECT_EQ(count, (number_ops * (number_ops - 1) / 2));
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



