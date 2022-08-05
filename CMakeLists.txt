# From https://github.com/TheLartians/ModernCppStarter
cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(
    dupcfinder
    VERSION 1.0.0
    DESCRIPTION "Fast duplicate finder"
    LANGUAGES CXX
)

option(DUPCFINDER_BUILD_TESTS "Build tests" OFF)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
        FATAL_ERROR
        "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif()

set(DEFAULT_BUILD_TYPE "RelWithDebInfo")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD 20)

    # Set output folders
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) 
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

    find_package(Doxygen OPTIONAL_COMPONENTS dot)
    if (Doxygen_FOUND)
        add_subdirectory(doc)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

include(config/CPM.cmake)

add_subdirectory(src) # Common library
add_subdirectory(app)

# Testing only available if this is the main app (BUILD_TESTING is defined when 
# including CTest) 
# Emergency override FMS_SCHEDULER_BUILD_TESTS provided as well
# if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING) OR DUPCFINDER_BUILD_TESTS)
#     add_subdirectory(test)
# endif()
