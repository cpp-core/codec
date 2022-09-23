cmake_minimum_required (VERSION 3.22 FATAL_ERROR)
project (codec VERSION 0.1 LANGUAGES C CXX)

# Make including this file idempotent
#
if(TARGET codec)
  return()
endif()

# Configuration variables.
#
# Build the tests and set directories if this is the master project
#
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CODEC_TEST ON CACHE BOOL "Generate the tests.")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_DOCS_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
  
  # Have cmake output the compilation commands
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Options for generating tests and documentation.
#
option(CODEC_TEST "Generate the tests." OFF)
option(CODEC_DOCS "Generate the docs." OFF)

# Add our dependencies
#
include(GNUInstallDirs)
find_package(cc REQUIRED)
find_package(libbz2 REQUIRED)

find_package(PkgConfig REQUIRED)
set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})
pkg_check_modules(ZSTD REQUIRED IMPORTED_TARGET libzstd)

# Output configuration
#
message("-- codec: Configuration: ${CMAKE_CURRENT_SOURCE_DIR}")
message("-- codec: Included from: ${CMAKE_SOURCE_DIR}")
message("-- codec: CODEC_TEST ${CODEC_TEST}")
message("-- codec: CODEC_DOCS ${CODEC_DOCS}")
message("-- codec: CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
message("-- codec: CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message("-- codec: cc: ${cc_DIR}")
message("-- codec: libbz2: ${libbz2_DIR}")
message("-- codec: zstd: ${ZSTD_LDFLAGS}")

# Compilation
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall)
add_compile_options(-stdlib=libc++)
add_compile_options(-O)
add_link_options(-stdlib=libc++)

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

set(LIBRARIES
  cc::cc
  libbz2::cxx-libbz2
  PkgConfig::ZSTD
  )

target_link_libraries(codec PUBLIC ${LIBRARIES})

target_include_directories(codec
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Optionally configure the tests
#
if(CODEC_TEST)
  enable_testing()
  if (NOT TARGET check)
    add_custom_target(check ${CMAKE_CTEST_COMMAND})
    add_custom_target(check_detail ${CMAKE_CTEST_COMMAND} -V)
  endif()
  add_subdirectory(test)
endif()

# Optionally configure the documentation
#
# if(CODEC_DOCS)
#   add_subdirectory(docs)
# endif()

# Installation
#
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/install_config.cmake)
install_config(codec)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/core/codec DESTINATION include/core)
