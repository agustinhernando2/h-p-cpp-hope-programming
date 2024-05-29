#!/bin/bash

# Obtenemos la ruta absoluta del directorio del proyecto
PROJECT_PATH=$(pwd)

echo $PROJECT_PATH

# Ruta del directorio build
BUILD_DIR="build"

# Verificar si el directorio build existe
if [ -d "$BUILD_DIR" ]; then
    # Recorrer todos los archivos y carpetas dentro del directorio build
    for item in "$BUILD_DIR"/* "$BUILD_DIR"/.[!.]* "$BUILD_DIR"/..?*; do
        # Si el nombre del archivo o carpeta no es "_deps"
        if [ "$(basename "$item")" != "_deps" ]; then
            # Eliminar el archivo o carpeta
            rm -rf "$item"
        fi
    done
else
    echo "El directorio $BUILD_DIR no existe."
fi

# Crea el directorio build y entra en él
mkdir -p build && cd build

# Configura el proyecto usando CMake con el generador Ninja
if [ "$1" == "test" ]; then
    cmake -GNinja -DRUNS_COVERAGE=1 -DRUNS_TESTS=1 ..
    ninja -j12
    ctest --output-on-failure -VV
    gcovr -r .. --html --html-details -o reports/coverage.html
elif [ "$1" == "debug" ]; then
    cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
    ninja -j12
else
    cmake -GNinja ..
    ninja -j12
fi
