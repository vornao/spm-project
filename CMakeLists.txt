cmake_minimum_required(VERSION 3.20)
project(spm_project)


set(CMAKE_CXX_STANDARD 17)

if (APPLE)
    # print message
    message(STATUS "Apple detected -> adding fastflow from ~/fastflow")
    include_directories(~/fastflow)
    link_directories(/usr/local/lib)
endif ()
# include fastflow if linux
if (UNIX AND NOT APPLE)
    # print message
    message(STATUS "Linux detected -> adding fastflow from /usr/local/include")
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif ()

# add -lpthread flag
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -lpthread")
add_definitions(-DNO_DEFAULT_MAPPING)

add_executable(
    spm_project
    src/main.cpp
    src/utils/huffman-commons.h
    src/utils/huffman-commons.cpp
    src/utils/utimer.cpp
    src/thread/HuffmanParallel.cpp
    src/thread/HuffmanParallel.h
    src/sequential/HuffmanSequential.cpp
    src/sequential/HuffmanSequential.h
    src/fastflow/HuffmanFastFlow.cpp
    src/fastflow/HuffmanFastFlow.h
    src/fastflow/HuffmanMonode.h
    src/fastflow/HuffmanMonode.cpp
)


