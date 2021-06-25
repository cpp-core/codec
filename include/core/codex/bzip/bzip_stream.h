// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/zstd/decompressor.h"
#include "core/zstd/compressor.h"

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
	    setg(d_.get().ptr_base(), d_.get().ptr_base(), d_.get().ptr_position());
	    return std::char_traits<CharT>::to_int_type(*this->gptr());
	}
	return std::char_traits<CharT>::eof();
    }
    
private:
    zstd::Decompressor<std::istream> d_;
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
    {
	setp(c_.put().ptr_base(), c_.put().ptr_capacity() - 1);
    }

    virtual ~zstd_ostreambuf() {
	c_.put().set_area(0, pptr() - pbase());
	c_.write();
	c_.close();
    }

    virtual zstd_ostreambuf::int_type overflow(zstd_ostreambuf::int_type value) override {
	char *ptr = c_.put().ptr_base();
	*pptr() = traits_type::to_char_type(value);
	c_.put().set_area(0, 1 + pptr() - ptr);
	c_.write();
	setp(ptr, c_.put().ptr_capacity() - 1);
	return traits_type::not_eof(value);
    }

private:
    zstd::Compressor<std::ostream> c_;
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
