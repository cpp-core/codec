// Copyright (C) 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <fmt/format.h>
#include "core/cc/detail/vector_or_string.h"
#include "core/mp/type_name.h"

namespace core::cc::queue
{

template<typename T>
class SourceSpSc
{
public:
    using Element = T;
    using Elements = core::vector_or_string_t<Element>;
    using ElementPtrs = std::vector<Element*>;

    SourceSpSc(const Element *begin, const Element *end)
	: m_ptr(begin)
	, m_end(end)
    { }

    template<class U = int, std::enable_if_t<std::is_same_v<T,char>,U> = 0>
    SourceSpSc(std::string_view storage)
	: SourceSpSc(storage.data(), storage.data() + storage.size())
    { }
    
    template<class U = int, std::enable_if_t<not std::is_same_v<T,char>,U> = 0>
    SourceSpSc(std::string_view storage)
	: SourceSpSc((const T*)storage.data(), (const T*)(storage.data() + storage.size()))
    {
	if ((sizeof(U) * storage.size()) % sizeof(T) != 0)
	    throw std::runtime_error
		(fmt::format("bad conversion: {} chars's is not an intergral number of {}'s",
			     storage.size(), core::mp::type_name<T>()));
    }
    
    SourceSpSc(const std::vector<T>& storage)
	: SourceSpSc(storage.data(), storage.data() + storage.size())
    { }

    template<class U>
    SourceSpSc(const std::vector<U>& storage)
	: SourceSpSc((const T*)storage.data(), (const T*)(storage.data() + storage.size()))
    {
	if ((sizeof(U) * storage.size()) % sizeof(T) != 0)
	    throw std::runtime_error
		(fmt::format("bad conversion: {} {}'s is not an intergral number of {}'s",
			     storage.size(), core::mp::type_name<U>(), core::mp::type_name<T>()));
    }
    
    bool empty() const
    { return m_ptr == m_end; }
    
    bool active() const
    { return m_ptr < m_end; }
    
    bool pop(Element& element)
    {
	if (m_ptr == m_end)
	    return false;
	element = *m_ptr++;
	return true;
    }
		 
    bool pop(Element *begin, Element *end, size_t& count)
    {
	if (m_ptr == m_end)
	    return false;
	
	count = std::min(m_end - m_ptr, end - begin);
	std::copy(m_ptr, m_ptr + count, begin);
	m_ptr += count;
	return true;
    }

private:
    const Element *m_ptr, *m_end;
};

SourceSpSc(std::string_view) -> SourceSpSc<char>;


}; // core::cc::queue
