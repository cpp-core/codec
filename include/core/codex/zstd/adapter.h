// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include "core/util/common.h"

namespace zstd {

template<class T>
struct InStreamAdapter {
    static size_t read(T& is, char *ptr, size_t count) {
	is.read(ptr, count);
	return is.gcount();
    }
};

template<class T>
concept QueuePop = requires(T a, char *p, size_t c) { a.pop(p, p, c); };

template<class T>
requires QueuePop<T>
struct InStreamAdapter<T> {
    static size_t read(T& queue, char *ptr, size_t count) {
	if (not queue.pop(ptr, ptr + count, count))
	    return 0;
	return count;
    }
};

template<class T>
struct OutStreamAdapter {
    static void write(T& os, const char *ptr, size_t count) { os.write(ptr, count); }
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
    static void write(T& queue, const char *ptr, size_t count) { queue.push(ptr, ptr + count); }
    static void finish(T& queue) { queue.push_sentinel(); }
};

}; // zstd

