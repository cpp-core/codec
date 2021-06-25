// Copyright (C) 2021 by Mark Melton
//

#include <istream>
#include <sstream>
#include "core/codex/bzip/decompressor.h"

namespace bzip {

template<class Source>
Decompressor<Source>::Decompressor(Source& source, size_t n)
    : source_(source)
    , stream_(new_stream())
    , get_(stream_->next_out, stream_->avail_out, n)
    , put_(stream_->next_in, stream_->avail_in, n)
{
    auto rc = BZ2_bzDecompressInit(stream_.get(), 0, 0);
    if (rc != BZ_OK)
	throw std::runtime_error(fmt::format("BZ2_bzDecompressInit: failed with {}", rc));
}

template<class Source>
Decompressor<Source>::~Decompressor() {
    if (stream_)
	close();
}

template<class Source>
void Decompressor<Source>::close() {
    if (not stream_)
	throw std::runtime_error("bzip::Decompressor:: stream already closed");
    auto rc = BZ2_bzDecompressEnd(stream_.get());
    if (rc != BZ_OK)
	throw std::runtime_error(fmt::format("BZ2_bzDecompressEnd: failed with {}", rc));
    stream_.reset();
}

template<class Source>
bool Decompressor<Source>::read_line(string& line) {
    line.clear();
    
    if (not stream_)
	return false;
    
    while (true) {
	
	while (get_.available()) {
	    auto c = get_.consume();
	    if (c == '\n') return true;
	    else line.push_back(c);
	}
	
	underflow();
	
	if (not get_.available())
	    return line.size() > 0;
    }
}

template<class Source>
bool Decompressor<Source>::underflow() {
    if (not stream_)
	return false;
    
    while (true) {
	if (not put_.available()) {
	    if (not put_.read(source_)) {
		close();
		return false;
	    }
	}
	get_.clear();
	
	auto rc = BZ2_bzDecompress(stream_.get());
	if (rc != BZ_OK and rc != BZ_STREAM_END)
	    throw std::runtime_error(fmt::format("BZ2_bzDecompress: failed with {}", rc));
	
	if (get_.available())
	    return true;
    }
}

}; // bzip

template class bzip::Decompressor<std::istream>;
template class bzip::Decompressor<std::stringstream>;
