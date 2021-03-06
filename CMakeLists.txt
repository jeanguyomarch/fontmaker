cmake_minimum_required(VERSION 2.8)
project(fontmaker C CXX)

set(PROJECT_DESCRIPTION "Generate bitmap fonts for embedded LCD")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Options
option(WITH_EXAMPLES "Include examples" ON)

# Check for freetype
find_package(PkgConfig)
pkg_check_modules(FREETYPE REQUIRED freetype2)

find_program(FLEX flex)
if (FLEX STREQUAL FLEX-NOTFOUND)
   message(FATAL_ERROR "flex not found")
endif ()

# Cmake tree
add_subdirectory(bin)
add_subdirectory(tests)

# Uninstall target
add_custom_target(uninstall
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake
)

