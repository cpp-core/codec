// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <vector>
#include "core/cc/ring/cursor.h"

namespace core::cc::ring {

// A Processor holds a Cursor that indicates the highest sequence
// number that has finished processing and is now available along with
// read and write barriers that constrain which sequence numbers can be
// claimed by the Processor.
//
// A Processor is parameterized by a Strategy which implements the
// methods for waiting for a sequence number to be published.
template<class Strategy>
class Processor {
public:
    Processor(size_t size = 1024)
	: size_(size)
	, strategy_(size) {
    }

    // Return the conceptual size of the ring being managed.
    size_t size() const {
	return size_;
    }

    // Return a pointer to the cursor for this processor. The cursor
    // represents the highest sequence number that has been completed
    // by this processor.
    Cursor *cursor() {
	return &cursor_;
    }

    // Return a reference to the read barriers.
    const CursorPtrs& read_barriers() const {
	return read_barriers_;
    }

    // Return a reference to the write barriers.
    const CursorPtrs& write_barriers() const {
	return write_barriers_;
    }

    // Add <cursor> as a read barrier for this processor; i.e.,
    // <cursor> represents a limit for the highest sequence number
    // that can be claimed by this processor.
    void add_read_barrier(Cursor *cursor) {
	read_barriers_.push_back(cursor);
    }

    // Add <cursor> as a write barrier for this processor;
    // i.e. <cursor> + size() represents a limit for the highest
    // sequence number that can be claimed by this processor.
    void add_write_barrier(Cursor *cursor) {
	write_barriers_.push_back(cursor);
    }

    // Block the calling thread until a sequence number can be
    // claimed. Return the claimed sequence number.
    sequence_t claim() { return claim(1).first; }

    // Block the calling thread until <count> sequence numbers can be
    // claimed. Return the range of claimed sequence numbers.
    sequence_range_t claim(size_t count) {
	return strategy_.claim(count, read_barriers_, write_barriers_);
    }

    // Claim as many sequence numbers as possible and return the
    // claimed range (possibly empty).
    sequence_range_t claim_all() {
	return strategy_.claim_all(read_barriers_, write_barriers_);
    }

    // Publish the specified sequence number. This indicates that all
    // sequence numbers up to and including this one are available for
    // down-stream operations.
    void publish(sequence_t no) {
	cursor_.set(no);
    }

    // Publish the specified range of sequence numbers. This indicates
    // that all sequence number up to and including this range are
    // available for down-stream operations.
    void publish(sequence_range_t range) {
	cursor_.set(range.second);
    }

    // Publish all sequence numbers that have been claimed. This
    // indicates that all sequence numbers that have been claimed are
    // available for down-stream operations.
    void publish_claimed();
    
private:
    size_t size_;
    Cursor cursor_;
    CursorPtrs read_barriers_, write_barriers_;
    Strategy strategy_;
};

}; // core::cc::ring
