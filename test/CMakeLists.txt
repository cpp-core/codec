cmake_minimum_required (VERSION 3.22 FATAL_ERROR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)

find_package(googletest REQUIRED)
find_package(Threads REQUIRED)

set(TESTS
  cc/queue/lockfree_mpmc
  cc/queue/lockfree_spsc
  cc/queue/mpmc
  cc/queue/read_from_stream
  cc/queue/sink_spsc
  cc/queue/source_spsc
  cc/queue/write_to_stream
  cc/ring/dependency_graph
  cc/barrier
  cc/deferred_scoped_task
  cc/executor
  cc/gate
  cc/latch
  cc/scoped_task
  cc/sequencer
  )

set(TEST_LIBRARIES
  cxx_core_cc
  googletest::cxx-googletest
  Threads::Threads)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/configure_tests.cmake)
configure_tests("core" "${TEST_LIBRARIES}" ${TESTS})
