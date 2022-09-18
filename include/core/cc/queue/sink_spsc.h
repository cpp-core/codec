// Copyright (C) 2019, 2022 by Mark Melton
//

#pragma once
#include "core/cc/detail/vector_or_string.h"

namespace core::cc::queue
{

template<class T>
class SinkSpSc
{
public:
    using Element = T;
    using Elements = vector_or_string_t<T>;

    SinkSpSc()
    { }

    SinkSpSc(size_t reserve)
    { m_data.reserve(reserve); }

    bool empty() const
    { return m_data.size() > 0; }

    void push(const Element& element)
    { m_data.push_back(element); }
    
    void push(const Element *begin, const Element *end)
    { m_data.insert(m_data.end(), begin, end); }

    void push_sentinel()
    { }

    Elements const& data() const
    { return m_data; }

private:
    Elements m_data;
};

}; // core::cc::queue
