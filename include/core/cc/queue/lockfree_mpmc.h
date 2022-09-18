// Copyright (C) 2019, 2022 by Mark Melton
//

#pragma once
#include <atomic>
#include <cassert>
#include <new>
#include <stdexcept>

namespace core::cc::queue
{

template <class T>
class LockFreeMpMc
{
public:
  explicit LockFreeMpMc(const size_t capacity = 4096)
      : m_capacity(capacity)
      , m_head(0)
      , m_tail(0)
      , m_sentinel(std::numeric_limits<size_t>::max())
    {
	if (m_capacity < 1)
	    throw std::invalid_argument("LockFreeMpMc queue capacity is less than 1");
	
	m_slots = new(std::align_val_t{kCacheLineSize}) Slot[capacity+1];
	if (m_slots == nullptr)
	    throw std::bad_alloc();
	
	for (size_t i = 0; i < m_capacity; ++i) 
	    new (&m_slots[i]) Slot();

	static_assert(sizeof(LockFreeMpMc<T>) % kCacheLineSize == 0,
		      "\n\n"
		      "LockFreeMpMc<T> size must be a multiple of cache line size to "
		      "prevent false sharing between adjacent queues"
		      "\n\n");
	
	static_assert(sizeof(Slot) % kCacheLineSize == 0,
		      "\n\n"
		      "LockFreeMpMc: Slot size must be a multiple of cache line size to prevent "
		      "false sharing between adjacent slots"
		      "\n\n");
	
	assert(reinterpret_cast<size_t>(m_slots) % kCacheLineSize == 0 &&
	       "\n\n"
	       "m_slots array must be aligned to cache line size to prevent false "
	       "sharing between adjacent slots"
	       "\n\n");
	
	assert(reinterpret_cast<char *>(&m_tail) - reinterpret_cast<char *>(&m_head) >=
               static_cast<std::ptrdiff_t>(kCacheLineSize) &&
	       "\n\n"
	       "LockFreeMpMc: head and tail must be a cache line apart to prevent false sharing"
	       "\n\n"
	       );
    }
    
    ~LockFreeMpMc() noexcept
    { delete [] m_slots; }

    LockFreeMpMc(const LockFreeMpMc &) = delete;
    LockFreeMpMc &operator=(const LockFreeMpMc &) = delete;
    
    template <typename... Args>
    void emplace(Args &&... args) noexcept
    {
	static_assert(std::is_nothrow_constructible<T, Args &&...>::value,
		      "\n\n"
		      "LockFreeMpMc: T must be nothrow constructible with Args&&..."
		      "\n\n"
		      );
	
	auto const head = m_head.fetch_add(1);
	auto &slot = m_slots[idx(head)];
	while (turn(head) * 2 != slot.turn.load(std::memory_order_acquire));

	slot.construct(std::forward<Args>(args)...);
	slot.turn.store(turn(head) * 2 + 1, std::memory_order_release);
    }
    
    template <typename... Args>
    bool try_emplace(Args &&... args) noexcept
    {
	static_assert(std::is_nothrow_constructible<T, Args &&...>::value,
		      "\n\n"
		      "LockFreeMpMc: T must be nothrow constructible with Args&&..."
		      "\n\n");
	
	auto head = m_head.load(std::memory_order_acquire);
	for (;;)
	{
	    auto &slot = m_slots[idx(head)];
	    if (turn(head) * 2 == slot.turn.load(std::memory_order_acquire))
	    {
		if (m_head.compare_exchange_strong(head, head + 1))
		{
		    slot.construct(std::forward<Args>(args)...);
		    slot.turn.store(turn(head) * 2 + 1, std::memory_order_release);
		    return true;
		}
	    }
	    else
	    {
		auto const prev = head;
		head = m_head.load(std::memory_order_acquire);
		if (head == prev)
		    return false;
	    }
	}
    }
    
    void push(const T &v) noexcept {
	static_assert(std::is_nothrow_copy_constructible<T>::value,
		      "\n\n"
		      "LockFreeMpMc: T must be nothrow copy constructible"
		      "\n\n");
	
	emplace(v);
    }
    
    template <typename P,
	      typename = typename std::enable_if<
		  std::is_nothrow_constructible<T, P &&>::value>::type>
    void push(P &&v) noexcept
    {
	emplace(std::forward<P>(v));
    }
    
    bool try_push(const T &v) noexcept {
	static_assert(std::is_nothrow_copy_constructible<T>::value,
		      "\n\n"
		      "LockFreeMpMc: T must be nothrow copy constructible"
		      "\n\n");
	return try_emplace(v);
    }
    
    template <class P,
	      class = class std::enable_if<std::is_nothrow_constructible<T, P &&>::value>::type>
    bool try_push(P &&v) noexcept
    {
	return try_emplace(std::forward<P>(v));
    }

    void push_sentinel()
    {
	m_sentinel.store(m_head.load(), std::memory_order_release);
    }
    
    bool pop(T &v) noexcept
    {
	auto const tail = m_tail.fetch_add(1);
	auto &slot = m_slots[idx(tail)];
	while (turn(tail) * 2 + 1 != slot.turn.load(std::memory_order_acquire))
	    if (tail >= m_sentinel.load(std::memory_order_acquire))
		return false;
	
	v = slot.move();
	slot.destroy();
	slot.turn.store(turn(tail) * 2 + 2, std::memory_order_release);
	return true;
    }
    
    bool try_pop(T &v) noexcept
    {
	auto tail = m_tail.load(std::memory_order_acquire);
	for (;;) {
	    auto &slot = m_slots[idx(tail)];
	    if (turn(tail) * 2 + 1 == slot.turn.load(std::memory_order_acquire))
	    {
		if (m_tail.compare_exchange_strong(tail, tail + 1))
		{
		    v = slot.move();
		    slot.destroy();
		    slot.turn.store(turn(tail) * 2 + 2, std::memory_order_release);
		    return true;
		}
	    }
	    else
	    {
		auto const prevTail = tail;
		tail = m_tail.load(std::memory_order_acquire);
		if (tail == prevTail) 
		    return false;
	    }
	}
    }
    
private:
    constexpr size_t idx(size_t i) const noexcept { return i % m_capacity; }
    constexpr size_t turn(size_t i) const noexcept { return i / m_capacity; }
    
// #ifdef __cpp_lib_hardware_interference_size
//     static constexpr size_t kCacheLineSize = std::hardware_destructive_interference_size;
// #else
    static constexpr size_t kCacheLineSize = 64;
// #endif
    
    struct Slot
    {
	~Slot() noexcept
	{
	    if (turn & 1) 
		destroy();
	}
	
	template <typename... Args>
	void construct(Args &&... args) noexcept
	{
	    static_assert(std::is_nothrow_constructible<T, Args &&...>::value,
			  "\n\n"
			  "LockFreeMpMc: T must be nothrow constructible with Args&&..."
			  "\n\n");
	    new (&storage) T(std::forward<Args>(args)...);
	}
	
	void destroy() noexcept
	{
	    static_assert(std::is_nothrow_destructible<T>::value,
			  "\n\n"
			  "LockFreeMpMc: T must be nothrow destructible"
			  "\n\n");
	    reinterpret_cast<T *>(&storage)->~T();
	}
	
	T &&move() noexcept { return reinterpret_cast<T &&>(storage); }
	
	alignas(kCacheLineSize) std::atomic<size_t> turn = {0};
	typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
    };
    
private:
    static_assert(std::is_nothrow_copy_assignable<T>::value or
		  std::is_nothrow_move_assignable<T>::value,
		  "\n\n"
		  "LockFreeMpMc: T must be nothrow copy or move assignable"
		  "\n\n");
    
    static_assert(std::is_nothrow_destructible<T>::value,
		  "\n\n"
		  "LockFreeMpMc: T must be nothrow destructible"
		  "\n\n");
    
    const size_t m_capacity;
    Slot *m_slots;

    alignas(kCacheLineSize) std::atomic<size_t> m_head;
    alignas(kCacheLineSize) std::atomic<size_t> m_tail;
    alignas(kCacheLineSize) std::atomic<size_t> m_sentinel;
};

}; // core::cc::queue
