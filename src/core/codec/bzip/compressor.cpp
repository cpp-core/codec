// Copyright (C) 2021, 2022 by Mark Melton
//
#include <fmt/format.h>
#include <ostream>
#include <sstream>
#include "core/codec/bzip/compressor.h"
#include "core/codec/bzip/new_stream.h"

namespace bzip {

template<class Sink>
Compressor<Sink>::Compressor(Sink& sink, size_t n)
    : sink_(sink)
    , stream_(new_stream())
    , get_(stream_->next_out, stream_->avail_out, n)
{
    auto rc = BZ2_bzCompressInit(stream_.get(), 1, 0, 0);
    if (rc != BZ_OK)
	throw std::runtime_error(fmt::format("BZ2_bzCompressInit: failed with {}", rc));
}

template<class Sink>
Compressor<Sink>::~Compressor() {
    if (stream_)
	close();
}

template<class Sink>
void Compressor<Sink>::close() {
    if (not stream_)
	throw std::runtime_error("bzip::Compressor: stream already closed");
    
    while (true) {
	get_.clear();
	
	auto rc = BZ2_bzCompress(stream_.get(), BZ_FINISH);
	if (rc != BZ_FINISH_OK and rc != BZ_STREAM_END)
	    throw std::runtime_error
		(fmt::format("BZ2_bzCompress(stream, BZ_FINISH): failed with {}", rc));

	get_.update();
	sink_.write(get_.data(), get_.size());
	
	if (rc == BZ_STREAM_END)
	    break;
    }
    auto rc = BZ2_bzCompressEnd(stream_.get());
    if (rc != BZ_OK)
	throw std::runtime_error
	    (fmt::format("BZ2_bzCompressEnd: failed with {}", rc));
    stream_.reset();
}

template<class Sink>
void Compressor<Sink>::write(const char *input, size_t input_len) {
    stream_->next_in = (char*)input;
    stream_->avail_in = input_len;
    
    while (stream_->avail_in > 0) {
	get_.clear();
	auto rc = BZ2_bzCompress(stream_.get(), BZ_RUN);
	if (rc != BZ_RUN_OK)
	    throw std::runtime_error
		(fmt::format("BZ2_bzCompress: failed with {}", rc));
	sink_.write(get_.data(), get_.size());
    }
}

}; // bzip

template class bzip::Compressor<std::ostream>;
template class bzip::Compressor<std::stringstream>;


