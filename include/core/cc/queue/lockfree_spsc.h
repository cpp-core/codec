// Copyright (C) 2019, 2022 by Mark Melton
//

#pragma once
#include <atomic>
#include <thread>
#include "core/cc/detail/vector_or_string.h"

namespace core::cc::queue
{

template<class T>
class LockFreeSpSc
{
public:
    constexpr static size_t kCacheLineSize = 64;
    using Element = T;
    using Elements = core::vector_or_string_t<T>;
    
    LockFreeSpSc(size_t capacity = 256 * 1024, size_t cache_capacity = 8 * 1024)
    {
	m_capacity = capacity;
	m_cache_capacity = cache_capacity;
	
	m_storage = std::make_unique<T[]>(capacity + 2 * cache_capacity + 4 * kCacheLineSize);
	m_ring_ptr = m_storage.get() + kCacheLineSize;
	
	m_push_ptr = m_ring_ptr + capacity + kCacheLineSize;
	m_push_idx = 0;
	
	m_pop_ptr = m_push_ptr + cache_capacity + kCacheLineSize;
	m_pop_idx = cache_capacity;

	m_head = 0;
	m_tail = 0;
	m_terminal = std::numeric_limits<size_t>::max();
    }

    size_t capacity() const
    { return m_capacity; }
    
    size_t cache_capacity() const
    { return m_cache_capacity; }

    bool pop_cache_empty() const
    { return m_pop_idx == cache_capacity(); }

    bool push_cache_full() const
    { return m_push_idx == cache_capacity(); }

    bool pop(Element& element)
   {
	if (pop_cache_empty())
	{
	    size_t count{cache_capacity()};
	    if (not get_from_ring(m_pop_ptr, count))
		return false;
	    m_pop_idx = cache_capacity() - count;
	}
	
	element = m_pop_ptr[m_pop_idx++];
	return true;
    }

    bool pop(Element *element, Element *end, size_t& count)
    {
	if (pop_cache_empty())
	{
	    size_t ring_count{cache_capacity()};
	    if (not get_from_ring(m_pop_ptr, ring_count))
		return false;
	    m_pop_idx = cache_capacity() - ring_count;
	}

	count = std::min(size_t(end - element), cache_capacity() - m_pop_idx);
	auto src = m_pop_ptr + m_pop_idx;
	std::copy(src, src + count, element);
	m_pop_idx += count;
	return true;
    }

    void push(const Element& element)
    {
	m_push_ptr[m_push_idx++] = element;
	if (push_cache_full())
	    flush();
    }

    void push(const Element *element, const Element *end)
    {
	while (element < end)
	{
	    if (m_push_idx == cache_capacity())
		flush();
	    
	    auto count = std::min(size_t(end - element), cache_capacity() - m_push_idx);
	    std::copy(element, element + count, m_push_ptr + m_push_idx);
	    element += count;
	    m_push_idx += count;
	}
    }

    void push(const Elements& elements)
    {
	push(elements.data(), elements.data() + elements.size());
    }

    void flush()
    {
	if (m_push_idx > 0)
	{
	    put_to_ring(m_push_ptr, m_push_idx);
	    m_push_idx = 0;
	}
    }

    void push_sentinel()
    {
	flush();
	m_terminal.store(m_head.load(std::memory_order_relaxed), std::memory_order_release);
    }

private:
    bool get_from_ring(Element *element, size_t& count)
    {
	const auto tail = m_tail.load(std::memory_order_relaxed);
	while (tail == m_head.load(std::memory_order_acquire) and
	       tail != m_terminal.load(std::memory_order_acquire))
	    std::this_thread::yield();

	if (tail == m_terminal.load(std::memory_order_acquire))
	    return false;

	count = std::min(count, m_head.load(std::memory_order_relaxed) - tail);
	auto src = m_ring_ptr + tail % capacity();
	std::copy(src, src + count, element + cache_capacity() - count);
	m_tail.store(tail + count, std::memory_order_release);
	return true;
    }

    void put_to_ring(Element *element, size_t count)
    {
	while (count > 0)
	{
	    const auto head = m_head.load(std::memory_order_relaxed);
	    auto next_head = head + 1;
	    size_t tail;
	    while (next_head == (tail = m_tail.load(std::memory_order_acquire)))
		std::this_thread::yield();

	    auto copy_count = std::min(count, capacity() - head + tail);
	    std::copy(element, element + copy_count, m_ring_ptr + head % capacity());
	    count -= copy_count;
	    m_head.store(head + copy_count, std::memory_order_release);
	}
    }
    
    alignas(kCacheLineSize) size_t m_capacity;
    size_t m_cache_capacity;
    Element *m_ring_ptr;
    alignas(kCacheLineSize) size_t m_push_idx;
    Element *m_push_ptr;
    alignas(kCacheLineSize) std::atomic<size_t> m_head;
    alignas(kCacheLineSize) std::atomic<size_t> m_terminal;
    alignas(kCacheLineSize) std::atomic<size_t> m_tail;
    alignas(kCacheLineSize) size_t m_pop_idx;
    Element *m_pop_ptr;
    std::unique_ptr<T[]> m_storage;
};

}; // core::cc::queue
