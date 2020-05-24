cmake_minimum_required(VERSION 3.11)

project(OpenBeatVR LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES src/*.cpp)
file(GLOB INC_FILES include/sphere/*.hpp)

add_library(sphere ${SRC_FILES} ${INC_FILES})
target_include_directories(sphere PUBLIC include)
