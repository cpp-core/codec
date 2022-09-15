// Copyright (C) 2021, 2022 by Mark Melton
//

#include <fstream>
#include "core/codex/zstd/zstd_stream.h"

namespace core {

struct zstd_ifstream_base {
    zstd_ifstream_base(const std::string& filename) : ifs_(filename) { }
    std::ifstream ifs_;
};

template<class CharT = char, class TraitT = std::char_traits<CharT>>
class zstd_ifstream : private zstd_ifstream_base, public zstd_istream<CharT, TraitT> {
public:
    zstd_ifstream(const std::string& filename, size_t n = 0)
	: zstd_ifstream_base(filename)
	, zstd_istream<CharT, TraitT>(ifs_, n) {
    }
};

struct zstd_ofstream_base {
    zstd_ofstream_base(const std::string& filename) : ofs_(filename) { }
    std::ofstream ofs_;
};

template<class CharT = char, class TraitT = std::char_traits<CharT>>
class zstd_ofstream : private zstd_ofstream_base, public zstd_ostream<CharT, TraitT> {
public:
    zstd_ofstream(const std::string& filename, size_t n = 0)
	: zstd_ofstream_base(filename)
	, zstd_ostream<CharT, TraitT>(ofs_, n) {
    }
};

}; // ns core
