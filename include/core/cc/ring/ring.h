// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <cassert>
#include <vector>
#include "core/cc/ring/sequence.h"

namespace core::cc::ring {

template<class T>
class Ring {
public:
    Ring(size_t size = 1024)
	: mask_(size - 1)
	, data_(size)
	, end_(FinalSequence) {
	assert((size & (size - 1)) == 0);
    }

    void resize(size_t size) {
	mask_ = size - 1;
	data_.resize(size);
	assert((size & (size - 1)) == 0);
    }
    
    size_t size() const { return data_.size(); }
    void end(sequence_t seq) { end_ = seq; }
    const sequence_t& end() const { return end_; }

    T& operator[](size_t idx) { return data_[idx & mask_]; }
    const T& operator[](size_t idx) const { return data_[idx & mask_]; }
    
    T& element(size_t idx) { return data_[idx & mask_]; }
    const T& element(size_t idx) const { return data_[idx & mask_]; }
    
private:
    size_t mask_;
    std::vector<T> data_;
    sequence_t end_;
};

}; // core::cc::ring
