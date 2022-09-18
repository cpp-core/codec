// Copyright (C) 2019, 2022 by Mark Melton
//

#pragma once
#include <memory>
#include <thread>

namespace core::cc::queue
{

template<class T>
class TunnelSpSc
{
public:
    using Element = T;

    TunnelSpSc()
	: m_pop_ptr(nullptr)
	, m_pop_end(nullptr)
	, m_pop_count(0)
	, m_terminal(0)
    { }
    
    bool pop(Element *element, Element *end, size_t& count)
    {
	m_pop_ptr.store(element, std::memory_order_release);
	m_pop_end.store(end, std::memory_order_release);
	while (m_pop_end.load(std::memory_order_acquire) != nullptr and
	       not m_terminal.load(std::memory_order_acquire))
	    std::this_thread::yield();
	
	if (m_terminal.load(std::memory_order_acquire))
	    return false;
	
	count = m_pop_count.load(std::memory_order_acquire);
	return true;
    }

    void push(const Element *element, const Element *end)
    {
	while (m_pop_end.load(std::memory_order_acquire) == nullptr)
	    std::this_thread::yield();

	auto dst_ptr = m_pop_ptr.load(std::memory_order_acquire);
	auto dst_end = m_pop_end.load(std::memory_order_acquire);
	auto count = std::min(size_t(dst_end - dst_ptr), size_t(end - element));
	std::copy(element, element + count, dst_ptr);

	m_pop_count.store(count, std::memory_order_release);
	m_pop_end.store(nullptr, std::memory_order_release);
    }

    void push_sentinel()
    {
	m_terminal.store(1, std::memory_order_release);
    }
    
private:
    std::atomic<Element*> m_pop_ptr;
    std::atomic<Element*> m_pop_end;
    std::atomic<size_t> m_pop_count;
    std::atomic<int> m_terminal;
};

}; // core::cc::queue
