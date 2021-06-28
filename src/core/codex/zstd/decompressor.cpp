// Copyright (C) 2021 by Mark Melton
//

#include <sstream>
#include <zstd.h>
#include "core/codex/zstd/decompressor.h"
#include "core/codex/zstd/adapter.h"
#include "core/codex/zstd/exception.h"
#include "core/concurrent/queue/lockfree_spsc.h"
#include "core/concurrent/queue/source_spsc.h"

namespace zstd
{

template<class Stream>
Decompressor<Stream>::Decompressor(Stream& is, size_t n)
    : is_(is)
    , zsd_(ZSTD_createDStream())
    , put_(n > 0 ? n : ZSTD_DStreamInSize())
    , get_(n > 0 ? n : ZSTD_DStreamOutSize())
{ }

template<class Stream>
Decompressor<Stream>::~Decompressor() {
    if (zsd_)
	close();
}

template<class Stream>
bool Decompressor<Stream>::read_line(string& line) {
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

template<class Stream>
bool Decompressor<Stream>::underflow() {
    if (zsd_ == nullptr)
	throw zstd::error("attempt to read from closed stream");

    while (true) {
	if (put().empty()) {
	    auto count = InStreamAdapter<Stream>::read(is_, put().begin(), put().capacity());
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

template<class Stream>
void Decompressor<Stream>::close() {
    if (zsd_ == nullptr)
	throw zstd::error("attempt to close already closed stream");
    ZSTD_freeDStream(zsd_);
    zsd_ = nullptr;
}

template class Decompressor<std::istream>;
template class Decompressor<std::stringstream>;
template class Decompressor<core::mt::queue::LockFreeSpSc<char>>;
template class Decompressor<core::mt::queue::SourceSpSc<char>>;

}; // zstd


