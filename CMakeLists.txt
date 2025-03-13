cmake_minimum_required(VERSION 3.22)
project(json-cpp)

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCE_FILES
  json-cpp/*.cpp
  json-cpp/*.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    include
  PRIVATE
    json-cpp
)
