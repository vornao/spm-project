cmake_minimum_required(VERSION 3.20)
project(spm_project)

set(CMAKE_CXX_STANDARD 17)

# set LD_PRELOAD
set(ENV{LD_PRELOAD} "~/Desktop/unipi/SPM/spm-project/lib/jemalloc/lib/libjemalloc.a")

if (APPLE)
    # print message
    set(JEMALLOC_ROOT "~/Desktop/unipi/SPM/spm-project/lib/jemalloc/")
    message(STATUS "Apple detected -> adding fastflow from ~/fastflow")
    message(STATUS "Apple detected -> adding jemalloc from ~/jemalloc")
    include_directories(~/fastflow)
    link_directories(/usr/local/lib)
    # include jemalloc
    include_directories("~/Desktop/unipi/SPM/spm-project/lib/jemalloc/include/")
    link_directories(/usr/local/lib)
endif ()
# include fastflow if linux
if (UNIX AND NOT APPLE)
    # print message
    set(JEMALLOC_ROOT "/home/l.miglior/project/spm-project/lib/jemalloc/lib")
    message(STATUS "Linux detected -> adding fastflow from /usr/local/include")
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)

    # include jemalloc
    include_directories(/usr/local/include/jemalloc)
    link_directories(/usr/local/lib)
endif ()

find_library(JEMALLOC_LIB jemalloc PATHS ${JEMALLOC_ROOT}/lib)

#check if library is found
if (JEMALLOC_LIB)
    message(STATUS "jemalloc found")
    message(STATUS "jemalloc library: ${JEMALLOC_LIB}")
else ()
    message(FATAL_ERROR "jemalloc not found")
endif ()

# add -lpthread flag
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -lpthread")
add_definitions(-DNO_DEFAULT_MAPPING)

# add define CHKFILE 
add_definitions(-DCHKFILE)

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

#target_link_libraries(spm_project ${JEMALLOC_LIB})




