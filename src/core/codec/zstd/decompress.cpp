// Copyright (C) 2018, 2019, 2021, 2022 by Mark Melton
//

#define ZSTD_STATIC_LINKING_ONLY
#include <sstream>
#include <zstd.h>
#include "core/codec/zstd/decompress.h"
#include "core/codec/zstd/decompressor.h"
#include "core/codec/zstd/adapter.h"
#include "core/codec/zstd/exception.h"
#include "core/cc/queue/lockfree_spsc.h"
#include "core/cc/queue/source_spsc.h"
#include "core/cc/queue/sink_spsc.h"
#include "core/pp/seq.h"
#include "core/pp/map.h"
#include "core/pp/product.h"

namespace zstd
{

std::string decompress(const char *input_buffer, size_t input_size)
{
    auto final_size = ZSTD_findDecompressedSize(input_buffer, input_size);

    if (final_size == ZSTD_CONTENTSIZE_ERROR)
	throw zstd::error("ZSTD_findDecompressedSize: unknown data format");
    
    if (final_size == ZSTD_CONTENTSIZE_UNKNOWN)
    {
	std::string str{input_buffer, input_size};
	std::stringstream ss{str};
	core::cc::queue::SinkSpSc<char> sink;
	decompress((std::istream&)ss, sink);
	return sink.data();
    }

    std::string buffer;
    buffer.resize(final_size);
    
    auto size = ZSTD_decompress(buffer.data(), buffer.size(), input_buffer, input_size);
    if (size != final_size)
	throw zstd::error("ZSTD_decompress: {}", ZSTD_getErrorName(size));
    
    return buffer;
}

std::string decompress(std::string_view str)
{
    return decompress(str.data(), str.size());
}

template<class InStream, class OutStream>
void decompress(InStream& is, OutStream&os) {
    Decompressor d{is};
    while (d.underflow())
	OutStreamAdapter<OutStream>::write(os, d.view().data(), d.view().size());
    OutStreamAdapter<OutStream>::finish(os);
}

}; // zstd

#define CODE(A,B) template void zstd::decompress(A&, B&);

#define CODE_SEQ(A) CODE(CORE_PP_HEAD_SEQ(A), CORE_PP_SECOND_SEQ(A))

#define SOURCE() (std::istream,				\
		  std::stringstream,			\
		  core::cc::queue::LockFreeSpSc<char>,	\
		  core::cc::queue::SourceSpSc<char>)

#define SINK() (std::ostream,				\
		std::stringstream,			\
		core::cc::queue::SinkSpSc<char>)

#define PRODUCT() CORE_PP_EVAL_CARTESIAN_PRODUCT_SEQ(SOURCE(), SINK())

CORE_PP_EVAL_MAP_SEQ(CODE_SEQ, PRODUCT())


