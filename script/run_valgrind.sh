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
done < <(find src/ include/ -type f \( -name ".cpp" -o -name ".hpp" -o -name ".h" -o -name ".c" \))

echo "Executing: lcov $arguments"
lcov $arguments