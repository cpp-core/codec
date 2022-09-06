// Copyright (C) 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <ostream>
#include "core/util/common.h"

namespace zstd
{

/// Compress the input using the **Zstandard** algorithm.
///
/// \param input_buffer Pointer to the input bytes.
/// \param input_size Number of input bytes.
/// \return The compressed bytes as a string
string compress(const char *input_buffer, size_t input_size);

/// Compress the inpuit using the **Zstandard** algorithm.
///
/// \param str A string_view representing the input bytes.
/// \return The compressed bytes as a string
string compress(string_view str);

/// Compress the input using the **Zstandard** algorithm.
///
/// \tparam InStream Readable stream.
/// \tparam OutStream Writable stream.
/// \param is The input stream (source of input bytes).
/// \param os The output stream (sink of output bytes).
template<class InStream, class OutStream>
void compress(InStream& is, OutStream& os);

/// Compress the input using the **Zstandard** algorithm.
///
/// \tparam C Container template class
/// \tparam T Container value type
/// \param storage A container representing the input bytes.
/// \return The compressed bytes as a string
template<template <class> class C, class T>
string compress(const C<T>& storage)
{ return compress((const char*)storage.data(), storage.size()); }

}; // zstd
