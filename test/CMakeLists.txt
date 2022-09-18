cmake_minimum_required (VERSION 3.22 FATAL_ERROR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

find_package(googletest REQUIRED)
find_package(Threads REQUIRED)

set(TESTS
  codec/bzip
  codec/filter
  codec/zstd
  codec/zstd_stream
  )

set(TEST_LIBRARIES
  cxx_core_codec
  googletest::cxx-googletest
  Threads::Threads)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/configure_tests.cmake)
configure_tests("core" "${TEST_LIBRARIES}" ${TESTS})
