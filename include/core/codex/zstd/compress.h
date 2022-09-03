// Copyright (C) 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <ostream>
#include "core/util/common.h"

namespace zstd
{

string compress(const char *input_buffer, size_t input_size);
string compress(string_view str);

template<class InStream, class OutStream>
void compress(InStream& is, OutStream& os);

template<template <class> class C, class T>
string compress(const C<T>& storage)
{ return compress((const char*)storage.data(), storage.size()); }

}; // zstd
