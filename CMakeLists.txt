# Minimum required version of cmake
cmake_minimum_required(VERSION 3.5)

# ===

set(ProjectName "Hideit")
set(ProjectVersion "1.0")
set(ExecutableName "hideit")

set(CMAKE_CXX_STANDARD 20)
set(CMakeFlags "")
set(CMakeDebugFlags "")

# ===

project("${ProjectName}" VERSION "${ProjectVersion}")

# Project root directory and the source directory src/
set(ProjectDir "${PROJECT_SOURCE_DIR}") # PROJECT_SOURCE_DIR is set by calls to project()
set(SourceDir "${ProjectDir}/src")

# ==

add_subdirectory(Command-Line-Parser)
add_subdirectory(Image-Library)
add_subdirectory(Compression-Library)
add_subdirectory(Encryption-Library)

# ===

set(Libraries "cliparser" "ilib" "complib" "enclib")

set(IncludeDirs "${SourceDir}" "${ProjectDir}/Command-Line-Parser/src" "${ProjectDir}/Image-Library/src" "${ProjectDir}/Compression-Library/src" "${ProjectDir}/Encryption-Library/src")

set(SourceFiles
	"${SourceDir}/Main.cpp"
	"${SourceDir}/Steg.cpp"
	"${SourceDir}/StegUtils.cpp"
	"${SourceDir}/BitIO.cpp"
)

set(HeaderFiles
	"${SourceDir}/Steg.h"
	"${SourceDir}/StegUtils.h"
	"${SourceDir}/BitIO.h"
)

# ===

set(CMAKE_CXX_FLAGS_DEBUG "${CMakeDebugFlags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMakeFlags}")

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

add_executable("${ExecutableName}" "${SourceFiles}" "${HeaderFiles}")

target_include_directories("${ExecutableName}" PRIVATE "${IncludeDirs}")

target_link_libraries("${ExecutableName}" PRIVATE "${Libraries}")

install(TARGETS ${ExecutableName} DESTINATION "${CMAKE_INSTALL_BINDIR}")