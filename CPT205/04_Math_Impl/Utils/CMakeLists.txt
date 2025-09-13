CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(cpt205)

cmake_policy(SET CMP0072 NEW)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

include_directories(
    include/
)


add_executable(cpt205
    src/main.cc
)

target_link_libraries(cpt205
    OpenGL::GL
    GLUT::GLUT
)
