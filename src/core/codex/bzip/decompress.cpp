// Copyright (C) 2021, 2022 by Mark Melton
//

#include <sstream>
#include "core/codex/bzip/decompress.h"
#include "core/codex/bzip/decompressor.h"

namespace bzip {

std::string ecompress(const std::string& str) {
    std::stringstream ss(str);
    bzip::Decompressor d{ss};
    std::string ustr;
    while (d.underflow())
	ustr += d.view();
    return ustr;
}

}; // bzip
