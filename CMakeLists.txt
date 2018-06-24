cmake_minimum_required(VERSION 3.10)
project(exam_deque)


include_directories(${exam_deque_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(exam_deque main.cpp my_deque.cpp my_deque.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -O3")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")-Wall -fprofile-arcs -ftest-coverage -fsanitize=address,undefined
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --std=c++17 -D_GLIBCXX_DEBUG -g -O0 -fno-omit-frame-pointer -fsanitize=address,undefined")

add_executable(test_deque
        gtest/gtest.h
        gtest/gtest-all.cc
        gtest/gtest_main.cc
        my_deque.cpp
        my_deque.h
        counted.cpp
        counted.h
        fault_injection.cpp
        fault_injection.h testing.cpp)