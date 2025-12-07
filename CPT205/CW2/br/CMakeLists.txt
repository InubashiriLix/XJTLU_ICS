cmake_minimum_required(VERSION 3.10)
project(br2077_city CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(IntegratedCity
    src/IntegratedCity.cpp
)

target_link_libraries(IntegratedCity PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)
