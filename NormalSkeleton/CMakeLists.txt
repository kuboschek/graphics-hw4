cmake_minimum_required(VERSION 2.8)

project(Skeleton)

find_package( OpenGL REQUIRED)
find_package( GLUT REQUIRED)

add_executable( Skeleton OpenglSkeleton.h Skeleton.cpp ddsbase.cpp )

target_link_libraries( Skeleton ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}   )

