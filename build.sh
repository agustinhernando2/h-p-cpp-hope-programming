#!/bin/bash

# Obtenemos la ruta absoluta del directorio del proyecto
PROJECT_PATH=$(pwd)

echo $PROJECT_PATH

# Elimina el directorio build si existe, excepto la carpeta "_deps"
find build/* -maxdepth 0 -type d \( ! -name '_deps' \) -exec rm -rf {} +

# Crea el directorio build y entra en él
mkdir -p build && cd build

# Configura el proyecto usando CMake con el generador Ninja
if [ "$1" == "test" ]; then
    cmake -GNinja -DRUN_COVERAGE=1 -DRUN_TESTS=1 ..
    ninja -j12
    ctest --test-dir tests -VV 
elif [ "$1" == "debug" ]; then
    cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
    ninja
else
    cmake -GNinja ..
    ninja -j12
fi
