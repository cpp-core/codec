// Copyright (C) 2019, 2021 by Mark Melton
//

#pragma once
#include <istream>
#include "core/codex/zstd/decompress.h"
#include "core/concurrent/scoped_task.h"
#include "core/concurrent/queue/lockfree_spsc.h"

namespace zstd
{

template<class T>
vector<T> decompress_as(string_view zdata)
{
    auto buffer = decompress(zdata);
    auto nelems = buffer.size() / sizeof(T);
    if (nelems * sizeof(T) != buffer.size())
 	throw std::runtime_error("decompress_as: non-integral number of elements");
    vector<T> vec(nelems);
    std::copy(buffer.begin(), buffer.end(), reinterpret_cast<char*>(&vec[0]));
    return vec;
}

template<class SourceQ, class T>
void decompress_to(SourceQ& source, vector<T>& container, size_t block_size = 1024)
{
    container.resize(block_size);
    auto ptr = (char*)container.data();
    auto end = ptr + sizeof(T) * container.size();
    size_t count{0}, byte_count{0};
    while (source.pop(ptr, end, byte_count))
    {
	count += byte_count / sizeof(T);
	container.resize(count + block_size);
	ptr = (char*)(container.data() + count);
	end = ptr + sizeof(T) * container.size();
    }
    container.resize(count);
}

template<class T>
void decompress_to(std::istream& is, vector<T>& container)
{
    core::mt::queue::LockFreeSpSc<char> queue;
    core::mt::scoped_task task([&]() { decompress(is, queue); });
    decompress(queue, container);
    task.wait();
}

}; // zstd
