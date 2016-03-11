cmake_minimum_required(VERSION 2.8)

project(MarchingCubes)

add_definitions(-Wno-deprecated)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(MarchingCubes OpenglSkeleton.h MarchingCubes.cpp ddsbase.cpp)

target_link_libraries(MarchingCubes ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
