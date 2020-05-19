cmake_minimum_required(VERSION 3.11)

project(OpenBeatVR LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

include(FetchContent)
function(fetch_extern name repo tag)
	FetchContent_Declare(
		${name}
		GIT_REPOSITORY ${repo}
		GIT_TAG ${tag}
	)
	FetchContent_GetProperties(${name})
	if (NOT ${name}_POPULATED)
		FetchContent_Populate(${name})
	endif()
	set(source_dir ${${name}_SOURCE_DIR})
	set(binary_dir ${${name}_BINARY_DIR})
	add_subdirectory(${source_dir} ${binary_dir} EXCLUDE_FROM_ALL)
	
	# Store source & binary dir as global variables
	set_property(GLOBAL PROPERTY ${name}_SOURCE_DIR ${source_dir})
	set_property(GLOBAL PROPERTY ${name}_BINARY_DIR ${binary_dir})
endfunction(fetch_extern)

function(fetch_extern_url name url)
	FetchContent_Declare(
		${name}
		URL ${url}
	)
	FetchContent_GetProperties(${name})
	if (NOT ${name}_POPULATED)
		FetchContent_Populate(${name})
	endif()
	set(source_dir ${${name}_SOURCE_DIR})
	set(binary_dir ${${name}_BINARY_DIR})
#	add_subdirectory(${source_dir} ${binary_dir} EXCLUDE_FROM_ALL)

	# Store source & binary dir as global variables
	set_property(GLOBAL PROPERTY ${name}_SOURCE_DIR ${source_dir})
	set_property(GLOBAL PROPERTY ${name}_BINARY_DIR ${binary_dir})
endfunction(fetch_extern_url)

fetch_extern(glm https://github.com/g-truc/glm 0.9.9.8)

# Fetch OpenAL
# Requires SNDFile
set(BUILD_PROGRAMS OFF)
set(BUILD_EXAMPLES OFF)
set(ENABLE_CPACK OFF)
set(BUILD_TESTING OFF)
set(BUILD_REGTEST OFF)
set(ENABLE_PACKAGE_CONFIG OFF)
fetch_extern(sndfile https://github.com/erikd/libsndfile 06ebde50e362966184790c1b53512438a4385d47)

set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_UTILS ON)
set(ALSOFT_INSTALL OFF)
set(ALSOFT_BUILD_ROUTER ON)

fetch_extern(openal https://github.com/kcat/openal-soft openal-soft-1.20.1)
get_property(openal_SOURCE_DIR GLOBAL PROPERTY openal_SOURCE_DIR)
set(OpenAl_INCLUDE_DIRECTORIES ${openal_SOURCE_DIR}/include)
# SndFile::sndfile
set(OpenAl_DEPS OpenAL sndfile)

# Fetch STB Image
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stb_image)
file(DOWNLOAD https://raw.githubusercontent.com/nothings/stb/master/stb_image.h ${CMAKE_CURRENT_BINARY_DIR}/stb_image/stb_image.h)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stb_image/stb_image.cpp 
	"// This file was generated with CMake --- DO NOT EDIT
#define STB_IMAGE_IMPLEMENTATION\n
#include \"stb_image.h\"")
add_library(stb_image ${CMAKE_CURRENT_BINARY_DIR}/stb_image/stb_image.h ${CMAKE_CURRENT_BINARY_DIR}/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/stb_image)

# Fetch GLAD
fetch_extern(glad https://github.com/Dav1dde/glad v0.1.33)

# Fetch OpenVR
set(BUILD_SHARED ON)
fetch_extern(openvr https://github.com/ValveSoftware/openvr v1.11.11)
get_property(openvr_SOURCE_DIR GLOBAL PROPERTY openvr_SOURCE_DIR)
include_directories(${openvr_SOURCE_DIR}/headers)
file(COPY ${openvr_SOURCE_DIR}/bin/win64/openvr_api.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("OpenVR: " ${openvr_SOURCE_DIR})

# Fetch FreeType
fetch_extern(freetype2 https://github.com/aseprite/freetype2 VER-2-10-0)

# Fetch ozz-animation
set(ozz_build_tools ON)
set(ozz_build_fbx OFF)
set(ozz_build_data OFF)
set(ozz_build_samples OFF)
set(ozz_build_howtos OFF)
set(ozz_build_testst OFF)
fetch_extern(ozz-animation https://github.com/guillaumeblanc/ozz-animation 0.12.1)
set(OZZ_LIBRARRIES ozz_base ozz_animation_offline ozz_animation ozz_geometry ozz_options)

find_package(GLFW3 REQUIRED)
message("GLFW3: " ${GLFW3_LIBRARY})
include_directories(${GLFW3_INCLUDE_DIR})

# Fetch JSON library
fetch_extern(json https://github.com/nlohmann/json v3.7.3)

file(GLOB SRC_FILES src/*.cpp)
file(GLOB INC_FILES include/openbeat*.hpp)

set(OPENBEAT_DEPS glad ${GLFW3_LIBRARY} glm OpenGL::GL openvr_api64 stb_image nlohmann_json::nlohmann_json freetype ${OZZ_LIBRARIES} ${OpenAl_DEPS})

add_executable(openbeat ${SRC_FILES} ${INC_FILES})
target_include_directories(openbeat PUBLIC include)
target_link_libraries(openbeat ${OPENBEAT_DEPS})

target_compile_definitions(openbeat PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
