cmake_minimum_required(VERSION 3.20)
project(spm_project)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        spm_project
        src/main.cpp
        src/sequential/huffman-sequential.h
        src/sequential/huffman-sequential.cpp
        src/thread/huffman-threads.h
        src/thread/huffman-threads.cpp
        src/utils/utils.h
        src/utils/huffman-commons.h
        src/utils/utils.cpp
        src/utils/huffman-commons.cpp)
