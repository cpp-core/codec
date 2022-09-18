// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <cstddef>

namespace zstd {

template<class T>
struct InStreamAdapter {
    static std::size_t read(T& is, char *ptr, std::size_t count) {
	is.read(ptr, count);
	return is.gcount();
    }
};

template<class T>
concept QueuePop = requires(T a, char *p, std::size_t c) { a.pop(p, p, c); };

template<class T>
requires QueuePop<T>
struct InStreamAdapter<T> {
    static std::size_t read(T& queue, char *ptr, std::size_t count) {
	if (not queue.pop(ptr, ptr + count, count))
	    return 0;
	return count;
    }
};

template<class T>
struct OutStreamAdapter {
    static void write(T& os, const char *ptr, std::size_t count) { os.write(ptr, count); }
    static void finish(T& os) {
	if constexpr (requires(T a) { a.close(); })
			 os.close();
    }
};

template<class T>
concept QueuePush = requires(T a, const char *p) { a.push(p, p); };

template<class T>
requires QueuePush<T>
struct OutStreamAdapter<T> {
    static void write(T& queue, const char *ptr, std::size_t count) { queue.push(ptr, ptr + count); }
    static void finish(T& queue) { queue.push_sentinel(); }
};

}; // zstd

