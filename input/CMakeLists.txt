# neic-glass3 input library CMake configuration file.
#
# This file contains the CMake configuration file that builds the input
# library for neic-glass3. This configuration follows the "CMake Superbuilds and
# Git Submodules" scheme to organize and orchestrate the build process.

cmake_minimum_required (VERSION 3.4)
set(CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../cmake/)

# ----- PROJECT VERSION ----- #
include(${CMAKE_DIR}/version.cmake)

# ----- PROJECT ----- #
project (input VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )

#----- BASE FUNCTIONS ----- #
include(${CMAKE_DIR}/base.cmake)

# ----- EXTERNAL LIBRARIES ----- #
# SuperEasyJSON
include(${CMAKE_DIR}/include_SuperEasyJSON.cmake)
# util
include(${CMAKE_DIR}/include_glass3_util.cmake)
# detectionformats
include(${CMAKE_DIR}/include_detectionformats.cmake)
# parse
include(${CMAKE_DIR}/include_glass3_parse.cmake)

# ----- Library boilerplate ----- #
include(${CMAKE_DIR}/glass3_library_boilerplate.cmake)

# DK REVIEW 20180730
# I would like to see the cmake files cleaned up a bit more.
# I *THINK* but am not certain, that the following pieces could
# be extracted into cmake include files that live in the base cmake
# directory.  This would shorten each libraries cmake, make it 
# more insulated from changes in included libraries, and make
# files even less redundant, reducing the number of places things
# need to be changed if stuff changes.


# New files to be added to cmake directory, that are referenced above:

# include_SuperEasyJSON.cmake:
find_package(SuperEasyJSON CONFIG REQUIRED)
include_directories(${SuperEasyJSON_INCLUDE_DIRS})

# include_glass3_util.cmake:
find_package(util CONFIG REQUIRED)
include_directories(${util_INCLUDE_DIRS})
include_directories(${spdlog_INCLUDE_DIRS}/..)
include_directories(${spdlog_INCLUDE_DIRS})

# include_detectionformats.cmake:
find_package(DetectionFormats CONFIG REQUIRED)
include_directories(${DetectionFormats_INCLUDE_DIRS})
include_directories(${RapidJSON_INCLUDE_DIRS})

# include_parse.cmake:
find_package(parse CONFIG REQUIRED)
include_directories(${parse_INCLUDE_DIRS})

# glass3_library_boilerplate.cmake:
#-----------------------------------------------------#
# ----- SET INCLUDE DIRECTORIES ----- #
include_directories ("${PROJECT_BINARY_DIR}")
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----- SET SOURCE FILES ----- #
file(GLOB SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp")

# ----- SET HEADER FILES ----- #
file(GLOB HDRS "${PROJECT_SOURCE_DIR}/include/*.h")

# ----- BUILD LIBRARY ----- #
include(${CMAKE_DIR}/build_lib.cmake)

# ----- GENERATE DOCUMENTATION ----- #
set(DOC_DIRS "${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src")
include(${CMAKE_DIR}/documentation.cmake)

# ----- RUN CPPCHECK ----- #
include(${CMAKE_DIR}/cppcheck.cmake)

# ----- RUN CPPLINT ----- #
include(${CMAKE_DIR}/cpplint.cmake)

# ----- RUN UNIT TESTS ----- #
file(GLOB TESTS ${PROJECT_SOURCE_DIR}/tests/*.cpp)
# WARNING: linking Order of libraries  matters for G++
# set(TEST_LIBRARIES ${parse_LIBRARIES} ${DetectionFormats_LIBRARIES} ${util_LIBRARIES} ${SuperEasyJSON_LIBRARIES})
# include(${CMAKE_DIR}/test.cmake)

# ----- INSTALL LIBRARY ----- #
include(${CMAKE_DIR}/install_lib.cmake)

#-----------------------------------------------------#
# end - glass3_library_boilerplate.cmake:
