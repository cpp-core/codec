cmake_minimum_required (VERSION 3.22 FATAL_ERROR)
project (cxx_core_codec VERSION 0.1 LANGUAGES C CXX)

# Make including this file idempotent
#
if(TARGET cxx_core_codec)
  return()
endif()

# Configuration variables.
#
# Build the tests and set directories if this is the master project
#
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CORE_CODEC_TEST ON CACHE BOOL "Generate the tests.")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_DOCS_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
  
  # Have cmake output the compilation commands
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Options for generating tests and documentation.
#
option(CORE_CODEC_TEST "Generate the tests." OFF)
option(CORE_CODEC_DOCS "Generate the docs." OFF)

set(CMAKE_MINIMUM_REQUIRED_VERSION 3.22)
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
message("-- cxx-core-codec: CMAKE_MINIMUM_REQUIRED_VERSION: ${CMAKE_MINIMUM_REQUIRED_VERSION}")
message("-- cxx-core-codec: PKG_CONFIG_USE_CMAKE_PREFIX_PATH: ${PKG_CONFIG_USE_CMAKE_PREFIX_PATH}")

# Add our dependencies
#
include(GNUInstallDirs)
find_package(cxx_core_cc REQUIRED)
find_package(libbz2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZSTD REQUIRED libzstd)

# Output configuration
#
message("-- cxx-core-codec: Configuration: ${CMAKE_CURRENT_SOURCE_DIR}")
message("-- cxx-core-codec: Included from: ${CMAKE_SOURCE_DIR}")
message("-- cxx-core-codec: CORE_CODEC_TEST ${CORE_CODEC_TEST}")
message("-- cxx-core-codec: CORE_CODEC_DOCS ${CORE_CODEC_DOCS}")
message("-- cxx-core-codec: CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
message("-- cxx-core-codec: cxx_core_cc: ${cxx_core_cc_DIR}")
message("-- cxx-core-codec: libbz2: ${libbz2_DIR}")
message("-- cxx-core-codec: zstd: ${ZSTD_LDFLAGS}")

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

add_library(cxx_core_codec ${FILES})

set(LIBRARIES
  cxx_core_cc::cxx_core_cc
  libbz2::cxx-libbz2
  ${ZSTD_LDFLAGS}
  )

target_link_libraries(cxx_core_codec PUBLIC ${LIBRARIES})

target_include_directories(cxx_core_codec
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Optionally configure the tests
#
if(CORE_CODEC_TEST)
  enable_testing()
  if (NOT TARGET check)
    add_custom_target(check ${CMAKE_CTEST_COMMAND})
    add_custom_target(check_detail ${CMAKE_CTEST_COMMAND} -V)
  endif()
  add_subdirectory(test)
endif()

# Optionally configure the documentation
#
# if(CORE_CODEC_DOCS)
#   add_subdirectory(docs)
# endif()

# Installation
#
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/install_config.cmake)
install_config(cxx_core_codec)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/core/codec DESTINATION include/core)
