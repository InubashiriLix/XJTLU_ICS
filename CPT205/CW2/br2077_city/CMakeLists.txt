cmake_minimum_required(VERSION 3.10)
project(br2077_city CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(Mesa
    src/Mesa.cpp
)

add_executable(LAPD 
    src/main_LAPD.cpp
)

add_executable(Dam 
    src/Dam.cpp
)

add_executable(Vegas
    src/Vegas.cpp
)

target_link_libraries(Mesa PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)
target_link_libraries(LAPD PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)
target_link_libraries(Dam PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)
target_link_libraries(Vegas PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)
