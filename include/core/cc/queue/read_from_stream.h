// Copyright (C) 2019, 2022 by Mark Melton
//

#pragma once
#include <string>
#include <istream>

namespace core::cc::queue
{

template<class SinkQ>
void read_from_stream(std::istream& is, SinkQ& sink)
{
    std::string buffer;
    buffer.resize(65536);
    while (is)
    {
	is.read(buffer.data(), buffer.size());
	sink.push(buffer.data(), buffer.data() + is.gcount());
    }
    sink.push_sentinel();
}

}; // core::cc::queue

