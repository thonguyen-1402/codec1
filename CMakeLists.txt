cmake_minimum_required(VERSION 3.2)
project (pcc_codec)
include_directories((${CMAKE_BUILD_DIRECTORY}/../src5))
set(CMAKE_BINARY_DIR ${CMAKE_BUILD_DIRECTORY}/../src5)
add_executable(encode  ${CMAKE_SOURCE_DIR}/../src5/octree.h
${CMAKE_SOURCE_DIR}/../src5/octree.cpp
${CMAKE_SOURCE_DIR}/../src5/encode.cpp
)
add_executable (decode  ${CMAKE_SOURCE_DIR}/../src5/octree.h
${CMAKE_SOURCE_DIR}/../src5/octree.cpp
${CMAKE_SOURCE_DIR}/../src5/decode.cpp)

