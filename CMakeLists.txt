
# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name and language
project(hgdb C)

# Set compiler flags: debug symbols, no optimization, suppress warnings, and allow common symbols
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -w -fcommon")

# ExternalProject for libmigdb
include(ExternalProject)
ExternalProject_Add(libmigdb_ext
    GIT_REPOSITORY https://github.com/cmelnu/libmigdb.git
    GIT_TAG main
    PREFIX ${CMAKE_BINARY_DIR}/libmigdb
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""          # No configure step (plain Make project)
    BUILD_IN_SOURCE 1               # Build inside cloned tree
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -C src
    INSTALL_COMMAND ""             # No separate install phase needed
)

# Obtain the source dir of the external project and create an imported target
ExternalProject_Get_Property(libmigdb_ext SOURCE_DIR)
set(LIBMIGDB_SOURCE_DIR ${SOURCE_DIR})
set(LIBMIGDB_LIB ${LIBMIGDB_SOURCE_DIR}/src/libmigdb.a)

add_library(migdb STATIC IMPORTED GLOBAL)
set_target_properties(migdb PROPERTIES IMPORTED_LOCATION ${LIBMIGDB_LIB})
add_dependencies(migdb libmigdb_ext)

# Define the source files for the project, now located in src/ folder
set(SOURCES
    src/hgdb.c
    src/error.c
    src/parse.c
    src/utils.c
)

# Include directories for header files (project + installed libmigdb headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# Headers from external project (directly from its src folder where mi_gdb.h lives)
include_directories(${LIBMIGDB_SOURCE_DIR}/src)

# Create an executable named 'hgdb' from the specified source files
add_executable(hgdb ${SOURCES})

# Also add target-specific include directories (modern CMake style)
target_include_directories(hgdb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${LIBMIGDB_SOURCE_DIR}/src)

# Add dependency so hgdb builds after libmigdb_ext
add_dependencies(hgdb libmigdb_ext)

# Path to the installed static library
target_link_libraries(hgdb migdb)

# Link the built migdb library
target_link_libraries(hgdb ${LIBMIGDB_LIB})