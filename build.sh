#!/bin/bash


# We only make check, not changes
# Use the find command with the variable
PROJECT_PATH=$(pwd)

echo $PROJECT_PATH

# sleep 1
# Elimina el directorio build si existe
rm -rf build

# Crea el directorio build y entra en él
mkdir build && cd build

# Configura el proyecto usando CMake con el generador Ninja
if [ "$1" == "test" ]; then
    cmake -GNinja -DRUN_COVERAGE=1 -DRUN_TESTS=1 ..
    ninja
    ctest --test-dir tests -VV 
elif [ "$1" == "debug" ]; then
    cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
    ninja
else
    cmake -GNinja ..
    ninja
fi
