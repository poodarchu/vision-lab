cmake_minimum_required(VERSION 3.7)
project(OpenCV_Basics)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp SIFT.cpp SIFT.h KeyPoint.h Descriptor.h SURF_test.h ORB_test.h RandomFerns.h SIFT_test.h)

add_executable(OpenCV_Basics ${SOURCE_FILES})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_LIBS})
target_link_libraries(OpenCV_Basics ${OpenCV_LIBRARIES})