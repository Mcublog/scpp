cmake_minimum_required(VERSION 3.0.0)
project(tests VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(tests test/crc_test.c crc32.c)
# Include paths
target_include_directories(tests PRIVATE test ${PROJECT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
