CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(ass1)

cmake_policy(SET CMP0072 NEW)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

include_directories(
    include/
)


add_executable(ass1
    src/main.cpp
)

target_link_libraries(ass1
    OpenGL::GL
    GLUT::GLUT
)
