cmake_minimum_required (VERSION 3.24 FATAL_ERROR)
project (codec VERSION 0.1 LANGUAGES C CXX)

# Make including this file idempotent
#
if(TARGET codec OR TARGET codec::codec)
  return()
endif()

# If this is the current project.
#
if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  # Laod the cmake recipes and utilities.
  #
  include(${CMAKE_CURRENT_LIST_DIR}/cmake/load_cmake_helpers.cmake)

  # Options for generating tests and documentation.
  #
  option(CODEC_TEST "Generate the tests." ON)
  option(CODEC_DOCS "Generate the docs." OFF)

  # compile_commands.json
  #
  symlink_compile_commands()

  # Convenience targets for git operations
  #
  make_submodule_pull_target()
  
else()
  option(CODEC_TEST "Generate the tests." OFF)
  option(CODEC_DOCS "Generate the docs." OFF)
endif()

# Put executables in the top-level binary directory
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Output configuration
#
message("-- codec: Configuration: ${CMAKE_CURRENT_SOURCE_DIR}")
message("-- codec: Included from: ${CMAKE_SOURCE_DIR}")
message("-- codec: Install prefix: ${CMAKE_INSTALL_PREFIX}")
message("-- codec: test ${CODEC_TEST}")
message("-- codec: docs ${CODEC_DOCS}")

# Setup compilation before adding dependencies
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall)
add_compile_options(-stdlib=libc++)
add_compile_options(-O)
add_link_options(-stdlib=libc++)

# Add our dependencies
#
add_cc()
add_bzip2()
add_zstd()

# Buidl the library
#
set(SOURCES
  codec/base64
  codec/bzip/compress
  codec/bzip/compressor
  codec/bzip/decompress
  codec/bzip/decompressor
  codec/bzip/get_area
  codec/bzip/put_area
  codec/zstd/compress
  codec/zstd/compressor
  codec/zstd/decompress
  codec/zstd/decompressor
  codec/zstd/get_area
  codec/zstd/put_area
  )

foreach(NAME ${SOURCES})
  list(APPEND FILES "src/core/${NAME}.cpp")
endforeach()

add_library(codec ${FILES})
add_library(codec::codec ALIAS codec)

# Define the header files for this library.
#
file(GLOB_RECURSE PUBLIC_INCLUDE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} include/*.h)
target_sources(codec PUBLIC FILE_SET HEADERS BASE_DIRS include FILES ${PUBLIC_INCLUDE_FILES})

target_link_libraries(codec PUBLIC cc::cc libbz2::libbz2 zstd::libzstd_static)

# Optionally configure the tests
#
if(CODEC_TEST)
  add_gtest()
  add_subdirectory(test)
endif()

# Optionally configure the documentation
#
# if(CODEC_DOCS)
#   add_subdirectory(docs)
# endif()

# Installation
#
install_config(codec)
