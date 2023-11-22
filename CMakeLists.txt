# This CMakeLists.txt is only required for building and running tests
# It is not intended to be used when consuming testlib.hpp. Simply
# include the header in your sources as described in the README.md.

cmake_minimum_required(VERSION 3.10)
project(testlib)

include(CTest)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall -Werror)

include_directories(.)

add_subdirectory(examples)
add_subdirectory(tests)
