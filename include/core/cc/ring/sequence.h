// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <cstdint>
#include <utility>
#include <limits>

namespace core::cc::ring {

// Cache line size used to align member variables to avoid false
// sharing.
static constexpr std::size_t CacheLineSize{64};

// Integral type used to represent sequence numbers.
using sequence_t = std::int64_t;
using sequence_range_t = std::pair<sequence_t, sequence_t>;

// Initial sequence number
static constexpr sequence_t InitialSequence{-1LL};
static constexpr sequence_t FinalSequence{std::numeric_limits<sequence_t>::max()};

}; // core::cc::ring
