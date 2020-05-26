cmake_minimum_required(VERSION 3.11)

project(OpenBeatVR LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(GAME_MULTITHREADED OFF)

# Make the engine follow the game.
set(ENGINE_EXTRA_COMPILER_CHECKS ON)
set(ENGINE_WERROR ON)

set(ENGINE_ENABLE_MULTITHREADED ${GAME_MULTITHREADED})
set(ENGINE_ENABLE_AUDIO ON)
if (APPLE)
	set(ENGINE_ENABLE_VR OFF)
else()
	set(ENGINE_ENABLE_VR ON)
endif()
set(ENGINE_ENABLE_JSON ON)
set(ENGINE_ENABLE_TEXT ON)
set(ENGINE_MIN_GAME_OBJECT ON)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/engine)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/sphere)

if (ENGINE_ENABLE_VR)
	get_property(openvr_SOURCE_DIR GLOBAL PROPERTY openvr_SOURCE_DIR)
	set(CMAKE_INSTALL_RPATH "${openvr_SOURCE_DIR}/bin/win64")
#	file(COPY ${openvr_SOURCE_DIR}/bin/win64/openvr_api.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

file(GLOB VERTEX_SHADERS shaders/*.vert)
file(GLOB FRAGMENT_SHADERS shaders/*.frag)
file(GLOB GEOMETRY_SHADERS shaders/*.geom)

set(ALL_SHADERS ${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${GEOMETRY_SHADERS})

# This will compile all our shaders, and create a new target `test_game_shaders` that depends on all tests.
test_shaders_func("${ALL_SHADERS}" "test_game_shaders")

file(GLOB SRC_FILES src/*.cpp)
file(GLOB INC_FILES include/openbeat/*.hpp)

add_executable(openbeat ${SRC_FILES} ${INC_FILES} ${ALL_SHADERS})
target_include_directories(openbeat PUBLIC include)
target_link_libraries(openbeat engine sphere)

add_dependencies(openbeat "test_game_shaders")

target_compile_definitions(openbeat PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
