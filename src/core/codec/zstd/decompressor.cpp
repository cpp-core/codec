// Copyright (C) 2021, 2022 by Mark Melton
//

#include <sstream>
#include <fstream>
#include <zstd.h>
#include "core/codec/zstd/decompressor.h"
#include "core/codec/zstd/adapter.h"
#include "core/codec/zstd/exception.h"
#include "core/cc/queue/lockfree_spsc.h"
#include "core/cc/queue/source_spsc.h"

namespace zstd
{

template<class Source>
Decompressor<Source>::Decompressor(std::add_rvalue_reference_t<Source> is, size_t n)
    : is_(std::forward<Source>(is))
    , zsd_(ZSTD_createDStream())
    , put_(n > 0 ? n : ZSTD_DStreamInSize())
    , get_(n > 0 ? n : ZSTD_DStreamOutSize())
{ }

template<class Source>
Decompressor<Source>::~Decompressor() {
    if (zsd_)
	close();
}

template<class Source>
Decompressor<Source>::Decompressor(Decompressor&& other)
    : is_(std::forward<Source>(other.is_))
    , zsd_(std::exchange(other.zsd_, nullptr))
    , put_(std::move(other.put_))
    , get_(std::move(other.get_)) {
}

template<class Source>
Decompressor<Source>::Decompressor(Decompressor&& other, Source& is)
    : is_(std::forward<Source>(is))
    , zsd_(std::exchange(other.zsd_, nullptr))
    , put_(std::move(other.put_))
    , get_(std::move(other.get_)) {
}

template<class Source>
bool Decompressor<Source>::read_line(std::string& line) {
    line.clear();
    
    if (zsd_ == nullptr)
	return false;

    while (true) {
	while (get().available()) {
	    auto c = get().consume();
	    if (c == '\n') return true;
	    else line.push_back(c);
	}

	underflow();

	if (not get().available())
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

	auto n = std::min(requested - count, get_.size());
	memcpy(buffer + count, get_.data(), n);
	count += n;
	get_.discard(n);
    }
    return count;
}


template<class Source>
bool Decompressor<Source>::underflow() {
    if (zsd_ == nullptr)
	throw zstd::error("attempt to read from closed stream");

    while (true) {
	if (put().empty()) {
	    auto count = InStreamAdapter<Source>::read(is_, put().begin(), put().capacity());
	    put().update(0, count);

	    if (count == 0) {
		close();
		return false;
	    }
	}

	get().clear();
						   
	auto r = ZSTD_decompressStream(zsd_, get().buffer(), put().buffer());
	if (ZSTD_isError(r))
	    throw zstd::error("read: %s", ZSTD_getErrorName(r));
						   
	get().update();

	if (get().available())
	    return true;
    }
}

template<class Source>
void Decompressor<Source>::close() {
    if (zsd_ == nullptr)
	throw zstd::error("attempt to close already closed stream");
    ZSTD_freeDStream(zsd_);
    zsd_ = nullptr;
}

template class Decompressor<std::istream&>;
template class Decompressor<std::ifstream&>;
template class Decompressor<std::stringstream&>;
template class Decompressor<core::cc::queue::LockFreeSpSc<char>&>;
template class Decompressor<core::cc::queue::SourceSpSc<char>&>;

template class Decompressor<std::ifstream>;

}; // zstd


