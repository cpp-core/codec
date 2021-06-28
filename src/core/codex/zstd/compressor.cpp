// Copyright (C) 2021 by Mark Melton
//

#include <sstream>
#include "core/codex/zstd/compressor.h"
#include "core/codex/zstd/adapter.h"
#include "core/concurrent/queue/lockfree_spsc.h"
#include "core/concurrent/queue/sink_spsc.h"
#include "core/concurrent/queue/source_spsc.h"

namespace zstd
{

template<class Stream>
Compressor<Stream>::Compressor(Stream& os, size_t n)
    : os_(os)
    , zsc_(ZSTD_createCStream())
    , get_(n > 0 ? n : ZSTD_CStreamOutSize())
{ }

template<class Stream>
Compressor<Stream>::~Compressor() {
    if (zsc_)
	close();
}

template<class Stream>
void Compressor<Stream>::write() {
    if (zsc_ == nullptr)
	throw zstd::error("attempt to write to closed stream");
    
    while (not put().empty()) {
	auto r = ZSTD_compressStream(zsc_, get().zbuffer(), put().buffer());
	if (ZSTD_isError(r))
	    throw zstd::error("write: ", ZSTD_getErrorName(r));
	OutStreamAdapter<Stream>::write(os_, get().ptr_base(), get().position());
	count_ += get().position();
	get().reset();
    }
    put().clear();
}

template<class Stream>
void Compressor<Stream>::write(const char *begin, const char *end) {
    put().update(begin, end);
    write();
}

template<class Stream>
void Compressor<Stream>::close() {
    if (zsc_ == nullptr)
	throw zstd::error("attempt to close already closed stream");
    
    while (true) {
	auto r = ZSTD_endStream(zsc_, get().zbuffer());
	if (ZSTD_isError(r))
	    throw zstd::error("close: %s", ZSTD_getErrorName(r));
	OutStreamAdapter<Stream>::write(os_, get().ptr_base(), get().position());
	get().reset();
	if (r <= 0)
	    break;
    }
    OutStreamAdapter<Stream>::finish(os_);
    ZSTD_freeCStream(zsc_);
    zsc_ = nullptr;
}

template class Compressor<std::ostream>;
template class Compressor<std::stringstream>;
template class Compressor<core::mt::queue::LockFreeSpSc<char>>;
template class Compressor<core::mt::queue::SinkSpSc<char>>;

}; // zstd

