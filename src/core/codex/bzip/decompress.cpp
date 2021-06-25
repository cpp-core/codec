// Copyright (C) 2021 by Mark Melton
//

#include <sstream>
#include "core/codex/bzip/decompress.h"
#include "core/codex/bzip/decompressor.h"

namespace bzip {

string decompress(const string& str) {
    std::stringstream ss(str);
    bzip::Decompressor d{ss};
    string ustr;
    while (d.underflow())
	ustr += d.view();
    return ustr;
}

}; // bzip
