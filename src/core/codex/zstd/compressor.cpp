// Copyright (C) 2021 by Mark Melton
//

#include <sstream>
#include <fstream>
#include "core/codex/zstd/compressor.h"
#include "core/codex/zstd/adapter.h"
#include "core/concurrent/queue/lockfree_spsc.h"
#include "core/concurrent/queue/sink_spsc.h"
#include "core/concurrent/queue/source_spsc.h"

namespace zstd
{

template<class Sink>
Compressor<Sink>::Compressor(Sink& os, size_t n)
    : os_(os)
    , zsc_(ZSTD_createCStream())
    , get_(n > 0 ? n : ZSTD_CStreamOutSize())
{ }

template<class Sink>
Compressor<Sink>::~Compressor() {
    if (zsc_)
	close();
}

template<class Sink>
void Compressor<Sink>::write(const char *begin, const char *end) {
    if (zsc_ == nullptr)
	throw zstd::error("attempt to write to closed stream");

    put().update(begin, end);

    while (not put().empty()) {
	auto r = ZSTD_compressStream(zsc_, get().buffer(), put().buffer());
	if (ZSTD_isError(r))
	    throw zstd::error("write: ", ZSTD_getErrorName(r));
	get().update();
	OutStreamAdapter<Sink>::write(os_, get().data(), get().size());
	count_ += get().size();
	get().clear();
    }
    put().clear();
}

template<class Sink>
void Compressor<Sink>::close() {
    if (zsc_ == nullptr)
	throw zstd::error("attempt to close already closed stream");
    
    while (true) {
	auto r = ZSTD_endStream(zsc_, get().buffer());
	if (ZSTD_isError(r))
	    throw zstd::error("close: %s", ZSTD_getErrorName(r));
	get().update();
	OutStreamAdapter<Sink>::write(os_, get().data(), get().size());
	get().clear();
	if (r <= 0)
	    break;
    }
    OutStreamAdapter<Sink>::finish(os_);
    ZSTD_freeCStream(zsc_);
    zsc_ = nullptr;
}

template class Compressor<std::ostream>;
template class Compressor<std::ofstream>;
template class Compressor<std::stringstream>;
template class Compressor<core::mt::queue::LockFreeSpSc<char>>;
template class Compressor<core::mt::queue::SinkSpSc<char>>;

}; // zstd

