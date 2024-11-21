# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name and language
project(hgdb C)

# Set compiler flags: debug symbols, no optimization, suppress warnings, and allow common symbols
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -w -fcommon")

# Define the source files for the project, now located in src/ folder
set(SOURCES
    src/hgdb.c
    src/error.c
    src/parse.c
    src/utils.c
)

# Include directories for header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create an executable named 'hgdb' from the specified source files
add_executable(hgdb ${SOURCES})

# Link the 'migdb' library to the 'hgdb' executable
target_link_libraries(hgdb migdb)

# Define a custom 'clean-all' target that runs a CMake script
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_clean.cmake
)

# Generate a custom cmake_clean.cmake file with cleaning instructions
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cmake_clean.cmake
"
# Remove build artifacts
file(REMOVE_RECURSE
    \${CMAKE_BINARY_DIR}/hgdb
    \${CMAKE_BINARY_DIR}/CMakeFiles
    \${CMAKE_BINARY_DIR}/CMakeCache.txt
    \${CMAKE_BINARY_DIR}/cmake_install.cmake
    \${CMAKE_BINARY_DIR}/Makefile
)

# Find and remove additional files (backups and object files)
file(GLOB_RECURSE EXTRA_FILES
    \${CMAKE_SOURCE_DIR}/src/*~
    \${CMAKE_SOURCE_DIR}/src/examples/*~
    \${CMAKE_SOURCE_DIR}/src/stub/*.o
    \${CMAKE_SOURCE_DIR}/src/*.o
)

if(EXTRA_FILES)
    file(REMOVE \${EXTRA_FILES})
endif()
")

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOCS "Build documentation" ON)

# look for Doxygen package
find_package(Doxygen)

if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # Note: do not put "ALL" - this builds docs together with application EVERY TIME!
    add_custom_target( docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)