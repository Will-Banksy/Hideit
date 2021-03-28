# Minimum required version of cmake
cmake_minimum_required(VERSION 3.5)

# ===

set(ProjectName "Steganography")
set(ProjectVersion "1.0")
set(ExecutableName "SteganographyExe")

set(CMakeFlags "-std=c++17")
set(CMakeDebugFlags "-g")

# ===

project("${ProjectName}" VERSION "${ProjectVersion}")

# Project root directory and the source directory src/
set(ProjectDir "${PROJECT_SOURCE_DIR}") # PROJECT_SOURCE_DIR is set by calls to project()
set(SourceDir "${ProjectDir}/src")

# ===

set(Libraries "") # Example: -lglfw -lGL -ldl

set(SourceFiles
	"${SourceDir}/Main.cpp"

	"${SourceDir}/parser/CommandLineParser.cpp"
	"${SourceDir}/parser/CommandLineOption.cpp"
	"${SourceDir}/parser/CommandLineArg.cpp"
)

set(HeaderFiles
	"${SourceDir}/parser/CommandLineParser.h"
	"${SourceDir}/parser/CommandLineOption.h"
	"${SourceDir}/parser/CommandLineArg.h"
)

# ===

set(CMAKE_CXX_FLAGS_DEBUG "${CMakeDebugFlags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMakeFlags}")

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

add_executable("${ExecutableName}" "${SourceFiles}" "${HeaderFiles}")

target_include_directories("${ExecutableName}" PRIVATE "${SourceDir}")

target_link_libraries("${ExecutableName}" PRIVATE "${Libraries}")

install(TARGETS ${ExecutableName} DESTINATION "${CMAKE_INSTALL_BINDIR}")