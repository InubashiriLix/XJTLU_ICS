CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(BST)

if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif()

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

include_directories(
    include/
)

add_executable(BST
    src/main.cc
)
