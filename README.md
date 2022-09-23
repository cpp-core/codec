[![Build and Unit Test](https://github.com/cpp-core/codec/actions/workflows/build.yaml/badge.svg)](https://github.com/cpp-core/codec/actions/workflows/build.yaml)

# Codec

Base64, libbz2 and zstd translation / (de)compression.

## At A Glance

## Installation

### Using depends (Recommended)

    git clone https://github.com/cpp-core/depends
	mkdir depends/build && cd depends/build
    CC=clang-mp-11 CXX=clang++-mp-11 cmake -DCMAKE_INSTALL_PREFIX=$HOME/opt -DCORE_CODEC_TEST=ON ..
	make codec-check # Run tests
	make codec       # Build and install
