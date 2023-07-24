cmake_minimum_required(VERSION 3.20)
project(spm_project)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        spm_project
        src/main.cpp
        src/utils/utils.h
        src/utils/huffman-commons.h
        src/utils/utils.cpp
        src/utils/huffman-commons.cpp src/utils/utimer.cpp src/thread/HuffmanParallel.cpp src/thread/HuffmanParallel.h src/sequential/HuffmanSequential.cpp src/sequential/HuffmanSequential.h)
