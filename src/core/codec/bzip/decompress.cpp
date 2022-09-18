// Copyright (C) 2021, 2022 by Mark Melton
//

#include <sstream>
#include "core/codec/bzip/decompress.h"
#include "core/codec/bzip/decompressor.h"

namespace bzip {

std::string decompress(const std::string& str) {
    std::stringstream ss(str);
    bzip::Decompressor d{ss};
    std::string ustr;
    while (d.underflow())
	ustr += d.view();
    return ustr;
}

}; // bzip
