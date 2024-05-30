#!/bin/bash

# Directorio donde se encuentran los datos de cobertura
COVERAGE_DIR="build/tests"

# Archivo donde se guardará la información de cobertura
COVERAGE_FILE="app.info"

# Comando para capturar los datos de cobertura
lcov --directory "$COVERAGE_DIR" --capture --output-file "$COVERAGE_FILE"

# Verificar si se generó correctamente el archivo de cobertura
if [ -f "$COVERAGE_FILE" ]; then
    echo "¡Archivo de cobertura generado correctamente!"
    
    # Mostrar un resumen de la cobertura
    coverageData=($(lcov --list "$COVERAGE_FILE" | grep "TOTAL" | grep -oE '[0-9.]+%'))
    
    # Obtener el porcentaje de cobertura de líneas
    linesCoverage=$(echo "${coverageData[0]}" | sed 's/%//')
    
    # Verificar si la cobertura de líneas es mayor al 20%
    # print the coverage percentage
    echo "Coverage: ${linesCoverage}%"
    if (( $(echo "$linesCoverage > 20" | bc -l) )); then
        echo "¡El coverage es mayor al 20% (${linesCoverage}%)!"
    else
        echo "¡El coverage es menor o igual al 20% (${linesCoverage}%)!"
    fi
else
    echo "¡Error: No se pudo generar el archivo de cobertura!"
fi

# Ahora ejecutamos el comando geninfo con la opción --ignore-errors para ignorar el error de inconsistencia
# geninfo --ignore-errors inconsistent --output-filename app.info "$COVERAGE_DIR"