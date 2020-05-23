cmake_minimum_required(VERSION 3.11)

project(OpenBeatVR LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(GAME_MULTITHREADED OFF)

# Make the engine follow the game.
set(ENGINE_ENABLE_MULTITHREADED ${GAME_MULTITHREADED})
set(ENGINE_ENABLE_AUDIO ON)
set(ENGINE_ENABLE_VR OFF)
set(ENGINE_ENABLE_JSON ON)
set(ENGINE_ENABLE_TEXT ON)
set(ENGINE_MIN_GAME_OBJECT ON)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/engine)

if (ENGINE_ENABLE_VR)
	get_property(openvr_SOURCE_DIR GLOBAL PROPERTY openvr_SOURCE_DIR)
	set(CMAKE_INSTALL_RPATH "${openvr_SOURCE_DIR}/bin/win64")
#	file(COPY ${openvr_SOURCE_DIR}/bin/win64/openvr_api.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

file(GLOB SRC_FILES src/*.cpp)
file(GLOB INC_FILES include/openbeat/*.hpp)

add_executable(openbeat ${SRC_FILES} ${INC_FILES})
target_include_directories(openbeat PUBLIC include)
target_link_libraries(openbeat engine)

target_compile_definitions(openbeat PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
