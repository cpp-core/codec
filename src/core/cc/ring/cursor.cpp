// Copyright (C) 2021, 2022 by Mark Melton
//

#include "core/cc/ring/cursor.h"

namespace core::cc::ring {

// Return the minimum index across the given cursors.
sequence_t xmin(const Cursors& cursors) {
    sequence_t m = std::numeric_limits<sequence_t>::max();
    for (const auto& cursor : cursors)
	m = std::min(m, cursor.get());
    return m;
}

// Return the minimum index across the given cursors.
sequence_t xmin(const CursorPtrs& ptrs) {
    sequence_t m = std::numeric_limits<sequence_t>::max();
    for (const auto& ptr : ptrs)
	m = std::min(m, ptr->get());
    return m;
}

}; // core::cc::ring
