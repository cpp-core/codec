// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/common.h"

namespace core {

template<class Filter, class CharT = char, class TraitsT = std::char_traits<CharT>>
class filter_streambuf : public std::streambuf {
public:
    filter_streambuf(std::istream& sin, Filter&& filter)
	: sin_(sin)
	, filter_(std::move(filter))
    { }
    
    int underflow() {
	if (this->gptr() == this->egptr()) {
	    line_.clear();
	    while (std::getline(sin_, line_)) {
		if (sin_)
		    line_ += "\n";
		if (not filter_(line_)) {
		    line_.clear();
		    continue;
		}
		break;
	    }

	    this->setg(line_.data(), line_.data(), line_.data() + line_.size());
	}
	return this->gptr() == this->egptr()
	    ? std::char_traits<CharT>::eof()
	    : std::char_traits<CharT>::to_int_type(*this->gptr());
    }
    
private:
    std::istream& sin_;
    Filter filter_;
    string line_;
};

template<class Filter, class CharT = char, class TraitT = std::char_traits<CharT>>
class filter_istream : public std::basic_istream<CharT, TraitT> {
public:
    filter_istream(std::istream& sin, Filter&& filter)
	: std::basic_istream<CharT, TraitT>::basic_istream
	(new filter_streambuf<Filter, CharT, TraitT>(sin, std::move(filter)))
    { }

    ~filter_istream() {
	delete this->rdbuf();
    }

private:
};

}; // ns core
