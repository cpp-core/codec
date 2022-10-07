cmake_minimum_required (VERSION 3.22 FATAL_ERROR)

find_package(Threads REQUIRED)

set(TESTS
  codec/bzip
  codec/filter
  codec/zstd
  codec/zstd_stream
  )

set(TEST_LIBRARIES
  codec
  GTest::gtest
  Threads::Threads)

configure_tests("core" "${TEST_LIBRARIES}" ${TESTS})
