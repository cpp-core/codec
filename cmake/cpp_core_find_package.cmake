cmake_minimum_required (VERSION 3.24 FATAL_ERROR)

function(cpp_core_find_package NAME)
  cmake_parse_arguments(CCFP
    "EXCLUDE_FROM_ALL" # options
    "URL;VERSION" # single-value arguments
    "TARGETS" # multi-value arguments
    ${ARGN}
    )

  if (DEFINED CCFP_URL)
    set(URL ${CCFP_URL})
  else()
    set(URL https://github.com/cpp-core/${NAME})
  endif()

  if (DEFINED CCFP_VERSION)
    set(VERSION ${CCFP_VERSION})
  else()
    set(VERSION main)
  endif()

  if (${CCFP_EXCLUDE_FROM_ALL})
    set(EXCLUDE_FROM_ALL EXCLUDE_FROM_ALL)
  endif()

  if (DEFINED CCFP_TARGETS)
    set(TARGETS ${CCFP_TARGETS})
  else()
    set(TARGETS "${NAME};${NAME}::${NAME}")
  endif()

  foreach(target ${TARGETS})
    if (TARGET ${target})
      message("-- cpp_core_find_package: ${target} already exists")
      return()
    endif()
  endforeach()

  message("-- cpp_core_find_package:")
  message("-- cpp_core_find_package: NAME: ${NAME}")
  message("-- cpp_core_find_package: URL: ${URL}")
  message("-- cpp_core_find_package: VERSION: ${VERSION}")
  if (DEFINED EXCLUDE_FROM_ALL)
    message("-- cpp_core_find_package: EXCLUDE_FROM_ALL set")
  endif()
  
  find_package(${NAME} QUIET)

  foreach(target ${TARGETS})
    if (TARGET ${target})
      message("-- cpp_core_find_package: found ${target} installed here: ${${NAME}_DIR}")
      return()
    endif()
  endforeach()

  include(FetchContent)

  FetchContent_Declare(
    ${NAME}
    GIT_REPOSITORY ${URL}
    GIT_TAG ${VERSION}
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
    )

  FetchContent_GetProperties(${NAME})
  string(TOLOWER "${NAME}" lc_name)

  if (${lc_name}_POPULATED)
    message(FATAL_ERROR "-- cpp_core_find_package: ${NAME} unexpectedly populated")
    return()
  endif()
  
  if (NOT ${lc_name}_POPULATED)
    FetchContent_Populate(${NAME})
    add_subdirectory(${${lc_name}_SOURCE_DIR} ${${lc_name}_BINARY_DIR} ${EXCLUDE_FROM_ALL})
  endif()
  
endfunction()

function(cpp_core_find_package_local NAME URL)
  message("-- cpp_core_find_package_local NAME: ${NAME}")
  message("-- cpp_core_find_package_local URL: ${URL}")
  
  include(FetchContent)

  FetchContent_Declare(
    ${NAME}
    GIT_REPOSITORY ${URL}
    GIT_TAG main
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
    )
  FetchContent_MakeAvailable(${NAME})
  
endfunction()
