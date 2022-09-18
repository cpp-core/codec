// Copyright (C) 2019, 2021, 2022 by Mark Melton
//

#define ZSTD_STATIC_LINKING_ONLY
#include <sstream>
#include <zstd.h>
#include "core/codex/zstd/compress.h"
#include "core/codex/zstd/compressor.h"
#include "core/codex/zstd/adapter.h"
#include "core/codex/zstd/exception.h"
#include "core/cc/queue/lockfree_spsc.h"
#include "core/cc/queue/sink_spsc.h"
#include "core/cc/queue/source_spsc.h"
#include "core/pp/seq.h"
#include "core/pp/map.h"
#include "core/pp/product.h"


namespace zstd
{

std::string compress(const char *input_buffer, size_t input_size)
{
    auto max_size = ZSTD_compressBound(input_size);
    std::string buffer;
    buffer.resize(max_size);
    
    auto final_size = ZSTD_compress(&buffer[0], max_size, input_buffer, input_size, 1);
    
    if (ZSTD_isError(final_size))
	throw zstd::error("{}", ZSTD_getErrorName(final_size));
    
    buffer.resize(final_size);
    return buffer;
}

std::string compress(std::string_view str)
{
    return compress(str.data(), str.size());
}

template<class InStream, class OutStream>
void compress(InStream& is, OutStream& os) {
    Compressor c{os};
    auto n = ZSTD_CStreamInSize();
    auto block = std::make_unique<char[]>(n);
    auto ptr = block.get();
    while (auto count = InStreamAdapter<InStream>::read(is, ptr, n))
	c.write(ptr, ptr + count);
}

}; // zstd

#define CODE(A,B) template void zstd::compress(A&, B&);

#define CODE_SEQ(A) CODE(CORE_PP_HEAD_SEQ(A), CORE_PP_SECOND_SEQ(A))

#define SOURCE() (std::istream,				\
		  std::stringstream,			\
		  core::cc::queue::LockFreeSpSc<char>,	\
		  core::cc::queue::SourceSpSc<char>)

#define SINK() (std::ostream,				\
		std::stringstream,			\
		core::cc::queue::LockFreeSpSc<char>,	\
		core::cc::queue::SinkSpSc<char>)

#define PRODUCT() CORE_PP_EVAL_CARTESIAN_PRODUCT_SEQ(SOURCE(), SINK())

CORE_PP_EVAL_MAP_SEQ(CODE_SEQ, PRODUCT())

