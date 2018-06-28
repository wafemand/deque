cmake_minimum_required(VERSION 3.10)
project(deque)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")-Wall -fprofile-arcs -ftest-coverage -fsanitize=address,undefined
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --std=c++17 -D_GLIBCXX_DEBUG -g3 -O0 -fno-omit-frame-pointer -fsanitize=address,undefined")

include_directories(.)
include_directories(gtest)

add_executable(deque
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        counted.cpp
        counted.h
        fault_injection.cpp
        fault_injection.h
        my_deque.cpp
        my_deque.h
        tests.cpp)

target_link_libraries(deque -lpthread)
