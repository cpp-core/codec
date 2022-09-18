// Copyright (C) 2021 by Mark Melton
//

#include "core/codex/zstd/decompressor.h"
#include "core/codex/zstd/compressor.h"
#include "core/codex/util/buffer.h"

namespace core {

template<class CharT = char, class TraitsT = std::char_traits<CharT>>
class zstd_istreambuf : public std::streambuf {
public:
    zstd_istreambuf(std::istream& sin, size_t n = 0)
	: d_(sin, n)
    { }

    virtual ~zstd_istreambuf() {
    }
    
    virtual int underflow() override {
	if (d_.underflow())  {
	    auto begin = (char*)d_.view().data();
	    auto end = begin + d_.view().size();
	    setg(begin, begin, end);
	    return std::char_traits<CharT>::to_int_type(*this->gptr());
	}
	return std::char_traits<CharT>::eof();
    }
    
private:
    zstd::Decompressor<std::istream&> d_;
};

template<class CharT = char, class TraitT = std::char_traits<CharT>>
class zstd_istream : public std::basic_istream<CharT, TraitT> {
public:
    zstd_istream(std::istream& sin, size_t n = 0)
	: std::basic_istream<CharT, TraitT>::basic_istream(new zstd_istreambuf(sin, n))
    { }

    ~zstd_istream() {
	delete this->rdbuf();
    }

private:
};

template<class CharT = char, class TraitsT = std::char_traits<CharT>>
class zstd_ostreambuf : public std::streambuf {
public:
    zstd_ostreambuf(std::ostream& sout, size_t n = 0)
	: c_(sout, n)
	, area_(n)
    {
	clear();
    }

    virtual ~zstd_ostreambuf() {
	c_.write(pbase(), pptr());
	c_.close();
    }

    virtual zstd_ostreambuf::int_type overflow(zstd_ostreambuf::int_type value) override {
	*pptr() = traits_type::to_char_type(value);
	c_.write(pbase(), pptr() + 1);
	clear();
	return traits_type::not_eof(value);
    }

private:
    void clear() {
	setp(area_.begin(), area_.end() - 1);
    }
    
    zstd::Compressor<std::ostream&> c_;
    core::BufferedArea area_;
};

template<class CharT = char, class TraitT = std::char_traits<CharT>>
class zstd_ostream : public std::basic_ostream<CharT, TraitT> {
public:
    zstd_ostream(std::ostream& sout, size_t n = 0)
	: std::basic_ostream<CharT, TraitT>::basic_ostream(new zstd_ostreambuf(sout, n))
    { }

    ~zstd_ostream() {
	delete this->rdbuf();
    }

private:
};

}; // ns core
