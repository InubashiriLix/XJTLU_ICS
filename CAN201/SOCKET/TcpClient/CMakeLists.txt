CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(tcp_client)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

include_directories(
    include/
)

add_executable(tcp_client
    src/main.cc
)
