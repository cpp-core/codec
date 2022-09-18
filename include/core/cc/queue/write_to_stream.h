// Copyright (C) 2019, 2022 by Mark Melton
//

#pragma once
#include <string>
#include <ostream>

namespace core::cc::queue
{

template<class SourceQ>
void write_to_stream(SourceQ& source, std::ostream& os)
{
    std::string buffer;
    buffer.resize(65536);
    std::size_t count{0};
    while (source.pop(buffer.data(), buffer.data() + buffer.size(), count))
	os.write(buffer.data(), count);
}


}; // core::cc::queue

