cmake_minimum_required(VERSION 2.8)
project(fontmaker C)

set(PROJECT_DESCRIPTION "Generate bitmap fonts for embedded LCD")
set(PROJECT_URL "http://jeanguyomarch.github.io/fontmaker/")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Options
option(WITH_EXAMPLES "Include examples" ON)

# Check for flex
find_program(FLEX flex)
if (FLEX STREQUAL FLEX-NOTFOUND)
   message(FATAL_ERROR "flex not found")
endif ()

# Check for gperf
find_program(GPERF gperf)
if (GPERF STREQUAL GPERF-NOTFOUND)
   message(FATAL_ERROR "gperf not found")
endif ()

# Check for freetype
find_package(PkgConfig)
pkg_check_modules(FREETYPE REQUIRED freetype2)

# Cmake tree
add_subdirectory(bin)
add_subdirectory(tests)

# Uninstall target
add_custom_target(uninstall
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake
)
