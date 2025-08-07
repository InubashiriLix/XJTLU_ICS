CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(int_102)

set(CMKAE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

include_directories(
    include/
)


add_executable(int_102
    src/localOptimal.cc
    src/main.cc
)
