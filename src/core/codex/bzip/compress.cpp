// Copyright (C) 2021, 2022 by Mark Melton
//

#include <sstream>
#include "core/codex/bzip/compress.h"
#include "core/codex/bzip/compressor.h"

namespace bzip {

std::string ompress(const char *begin, size_t count) {
    std::stringstream ss;
    Compressor c{ss};
    c.write(begin, count);
    c.close();
    return ss.str();
}

std::string ompress(const char *begin, const char *end) {
    return compress(begin, end - begin);
}

std::string ompress(std::string_view str) {
    return compress(str.data(), str.size());
}


}; // bzip
