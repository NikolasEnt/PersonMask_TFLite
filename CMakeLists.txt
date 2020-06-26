cmake_minimum_required(VERSION 3.11.3)

set(CMAKE_CXX_STANDARD 17)
project(OPENCV_person_mask)

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.3)
    message(FATAL_ERROR "Require at least gcc-9.3.0")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -std=c++17 -pthread")

set(TENSORFLOW ${HOME}/dependencies/tensorflow/)


find_package(OpenCV 4.3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# Find all executables
include_directories(include)
file(GLOB SOURCES src/*.cpp)


# Add project executable
add_executable(person_mask ${SOURCES})
target_link_libraries(person_mask ${OpenCV_LIBRARIES})
set(TENSORFLOW_LITE ${TENSORFLOW}/tensorflow/lite/tools/make)
target_link_libraries(person_mask ${TENSORFLOW_LITE}/gen/linux_x86_64/lib/libtensorflow-lite.a)
target_include_directories(person_mask PRIVATE ${TENSORFLOW})
target_include_directories(person_mask PRIVATE ${TENSORFLOW_LITE}/downloads/absl)
target_include_directories(person_mask PRIVATE ${TENSORFLOW_LITE}/downloads/flatbuffers/include)

target_include_directories(person_mask PRIVATE include)
