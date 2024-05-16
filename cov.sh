#!/bin/bash

cd build
valgrind --leak-check=full ctest --output-on-failure
cd ..

arguments="--capture "

# Set working directory
if [[ -d "build/tests" ]]; then
arguments+="--directory build/tests "
fi


# Set output file
arguments+="--output-file build/coverage.info "

# # Disable branch coverage
arguments+="-rc lcov_branch_coverage=0 "

# Include test files
while IFS= read -r file; do
    arguments+="--include $file "
done < <(find src/ include/ lib/ -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \))

echo "Executing: lcov $arguments"
lcov $arguments

coverageData=($(lcov --list build/coverage.info | grep Total | grep -oE '[0-9.]+%'))

# Check if lines the coverage is greater than 20%
linesCoverage=$(echo "${coverageData[0]}" | sed 's/%//')
echo "Lines coverage is: $linesCoverage %"
if ! (( $(echo "$linesCoverage > 20" | bc -l) )); then
    echo "----------------------------------------"
    echo "FAILED: Lines coverage is lower than 20%"
    echo "----------------------------------------"
    exit 1
else
    echo "------------------------------------------"
    echo "PASSED: Lines coverage is greater than 20%"
    echo "------------------------------------------"
fi

# Check if functions coverage is greater than 20%
functionsCoverage=$(echo "${coverageData[1]}" | sed 's/%//')
echo "Functions coverage is: $functionsCoverage %"
if ! (( $(echo "$functionsCoverage > 20" | bc -l) )); then
    echo "---------------------------------------------"
    echo "FAILED: Functions coverage is lower than 20%"
    echo "--------------------------------------------"
    exit 1
else
    echo "----------------------------------------------"
    echo "PASSED: Functions coverage is greater than 20%"
    echo "----------------------------------------------"
fi
