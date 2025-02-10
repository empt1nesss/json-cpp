cmake_minimum_required(VERSION 3.22)
project(json-cpp)

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCE_FILES
  source/*.cpp
  source/*.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    include
)

