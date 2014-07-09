cmake_minimum_required(VERSION 2.8)

# Configure build
project(UAVStation)
set ( EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Configuration
set(MYPROG_NAME ${PROJECT_NAME})
set(MYPROG_VERSION_MAJOR 0)
set(MYPROG_VERSION_MINOR 1)

# Compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")
endif()

# Generate list of source files
file (
    GLOB_RECURSE
    SRC_FILES
    src/*
    )

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/src/config.h"
	)

# Add the binayr tree to the search path for include files
# so that we will find config.h
include_directories( "${PROJECT_BINARY_DIR}" )

add_executable(
	${PROJECT_NAME} 
    ${SRC_FILES}
	)

# Linking
target_link_libraries(
    ${PROJECT_NAME}
    lua
    )
