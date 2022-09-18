// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <thread>
#include "core/cc/ring/cursor.h"

namespace core::cc::ring {

// A claim strategy suitable for a single publishing thread.
class SingleThreadClaimStrategy {
public:
    SingleThreadClaimStrategy(size_t size)
	: size_(size) {
    }
    inline sequence_range_t claim(const size_t count,
			   const CursorPtrs& read_barriers,
			   const CursorPtrs& write_barriers);

    inline sequence_range_t claim_all(const CursorPtrs& read_barriers,
			       const CursorPtrs& write_barriers);

private:
    sequence_t size_;
    sequence_t to_be_claimed_{0};
};

sequence_range_t SingleThreadClaimStrategy::claim(const size_t count,
						  const CursorPtrs& read_barriers,
						  const CursorPtrs& write_barriers) {
    const sequence_t start = to_be_claimed_;
    const sequence_t end = (to_be_claimed_ += count);
    if (read_barriers.size() > 0)
	while (min(read_barriers) < end - 1)
	    std::this_thread::yield();
    if (write_barriers.size() > 0)
	while (min(write_barriers) + size_ < end)
	    std::this_thread::yield();
    return {start, end};
}

sequence_range_t SingleThreadClaimStrategy::claim_all(const CursorPtrs& read_barriers,
						      const CursorPtrs& write_barriers) {
    sequence_t start = to_be_claimed_;
    sequence_t end = to_be_claimed_ + size_;
    if (read_barriers.size() > 0) {
	auto mr = min(read_barriers);
	end = std::min(end, mr + 1);
    }
    if (write_barriers.size() > 0) {
	auto mw = min(write_barriers);
	end = std::min(end, mw + size_);
    }
    if (end > start)
	to_be_claimed_ += end - start;
    return {start, end};
}

}; // core::cc::ring
