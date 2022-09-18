// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <memory>
#include <vector>
#include "core/cc/ring/sequence.h"

namespace core::cc::ring {

// Cursor represents a sequence number providing thread-aware get, set
// and incr operations. The sequence number represents the highest
// numbered element available to be processed.
class Cursor {
public:
    Cursor(std::int64_t initial_seq = InitialSequence)
	: seq_(initial_seq) {
    }

    // Cursor is non-copyable.
    Cursor(const Cursor&) = delete;
    Cursor& operator=(const Cursor&) = delete;

    // Return the current location of the cursor. Store operations by
    // other threads using memory_order_release will be visible to
    // this load.
    sequence_t get() const { return seq_.load(std::memory_order_acquire); }

    // Return one past the current location of the cursor. Store operations by
    // other threads using memory_order_release will be visible to
    // this load.
    sequence_t end() const { return get() + 1; }

    // Set the current location of the cursor. Load operations by other
    // threads using memory_order_acquire will see this store.
    void set(sequence_t v) { seq_.store(v, std::memory_order_release); }

    // Increment and return the new cursor location.
    sequence_t incr(sequence_t delta) {
	return seq_.fetch_add(delta, std::memory_order_release) + delta;
    }

private:
    // Align the data member to reduce false sharing.
    alignas(CacheLineSize) std::atomic<sequence_t> seq_;
};

using Cursors = std::vector<Cursor>;
using CursorPtrs = std::vector<Cursor*>;

// Return the minimum index across the given cursors.
inline sequence_t min(const Cursors& cursors);

// Return the minimum index across the given cursors.
inline sequence_t min(const CursorPtrs& ptrs);

sequence_t min(const Cursors& cursors) {
    sequence_t m = std::numeric_limits<sequence_t>::max();
    for (const auto& cursor : cursors)
	m = std::min(m, cursor.get());
    return m;
}

// Return the minimum index across the given cursors.
sequence_t min(const CursorPtrs& ptrs) {
    sequence_t m = std::numeric_limits<sequence_t>::max();
    for (const auto& ptr : ptrs)
	m = std::min(m, ptr->get());
    return m;
}

}; // core::cc::ring
