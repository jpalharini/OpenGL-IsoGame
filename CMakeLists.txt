cmake_minimum_required(VERSION 3.10)
project(OpenGL_IsoGame)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(OpenGL_IsoGame
        CMakeLists.txt
        Includes.h
        main.cpp
        )

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
find_package (OpenGL REQUIRED)

target_link_libraries(OpenGL_IsoGame ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
target_include_directories(OpenGL_IsoGame PRIVATE ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} /usr/local/include/glm)