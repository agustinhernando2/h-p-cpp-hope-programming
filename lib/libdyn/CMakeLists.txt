# Request the minimum version of CMake, in case of lower version throws error
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Define your project
project(
  "DynamiCLib"
  VERSION 1.0.0
  DESCRIPTION "CMake C project template for operating systems II"
  LANGUAGES C
)

# Define the C standard, we are going to use std17
set(CMAKE_C_STANDARD 17)

# Include the include directory, where the header files are located
include_directories(include)
include_directories(../libsta/include)

# Include the cJSON library
include_directories(../../external/cJSON)

# Add the src directory, where the source files are located
file(GLOB_RECURSE SOURCES "src/*.c")

# Create the executable
add_library(${PROJECT_NAME} SHARED ${SOURCES})
