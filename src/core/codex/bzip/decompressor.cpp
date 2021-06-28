// Copyright (C) 2021 by Mark Melton
//

#include <istream>
#include <sstream>
#include "core/codex/bzip/decompressor.h"
#include "core/codex/zstd/adapter.h"

namespace bzip {

template<class Source>
Decompressor<Source>::Decompressor(Source& source, size_t n)
    : src_(source)
    , bz_(new_stream())
    , get_(bz_->next_out, bz_->avail_out, n)
    , put_(bz_->next_in, bz_->avail_in, n)
{
    auto rc = BZ2_bzDecompressInit(bz_.get(), 0, 0);
    if (rc != BZ_OK)
	throw std::runtime_error(fmt::format("BZ2_bzDecompressInit: failed with {}", rc));
}

template<class Source>
Decompressor<Source>::~Decompressor() {
    if (bz_)
	close();
}

template<class Source>
void Decompressor<Source>::close() {
    if (not bz_)
	throw std::runtime_error("bzip::Decompressor:: stream already closed");
    auto rc = BZ2_bzDecompressEnd(bz_.get());
    if (rc != BZ_OK)
	throw std::runtime_error(fmt::format("BZ2_bzDecompressEnd: failed with {}", rc));
    bz_.reset();
}

template<class Source>
bool Decompressor<Source>::read_line(string& line) {
    line.clear();
    
    if (not bz_)
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
size_t Decompressor<Source>::read_bytes(char *buffer, size_t requested) {
    size_t count{0};
    while (count < requested) {
	if (not get_.available()) {
	    if (not underflow())
		break;
	}
	auto v = get_.view();
	auto n = std::min(requested - count, v.size());
	memcpy(buffer + count, v.data(), n);
	count += n;
	get_.discard(n);
    }
    return count;
}

template<class Source>
bool Decompressor<Source>::underflow() {
    if (not bz_)
	return false;

    while (true) {
	if (put_.empty()) {
	    auto count = zstd::InStreamAdapter<Source>::read(src_, put_.begin(), put_.capacity());
	    put_.update(count);
	    
	    if (count == 0) {
		close();
		return false;
	    }
	}
	
	get_.clear();
	
	auto rc = BZ2_bzDecompress(bz_.get());
	if (rc != BZ_OK and rc != BZ_STREAM_END)
	    throw std::runtime_error(fmt::format("BZ2_bzDecompress: failed with {}", rc));

	get_.update();
	
	if (get_.available())
	    return true;
    }
}

}; // bzip

template class bzip::Decompressor<std::istream>;
template class bzip::Decompressor<std::stringstream>;
