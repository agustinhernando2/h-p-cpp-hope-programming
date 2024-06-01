#!/bin/bash

# Obtenemos la ruta absoluta del directorio del proyecto
PROJECT_PATH=$(pwd)

echo $PROJECT_PATH


SOURCE_FILES=$(find $PROJECT_PATH/src -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/src/client -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/src/server -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/include -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/libdyn/src -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/libdyn/include -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/libsta/src -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/libsta/include -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/cannyEdgeFilter/src -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/cannyEdgeFilter/include -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/rocksDbWrapper/src -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")
SOURCE_FILES+=$(find $PROJECT_PATH/lib/rocksDbWrapper/include -type f \( -name "*.cpp" -or -name "*.hpp" -or -name "*.h" -or -name "*.c" \) | tr "\n" " ")

clang-format -i $SOURCE_FILES
clang-format -n $SOURCE_FILES


 # We only make check, not changes
DOX_CONF_FILE=$(pwd)/Doxyfile

# Append to DOX_CONF_FILE input source directories 
{
    cat $DOX_CONF_FILE
    echo "INPUT" = $(pwd)/include $(pwd)/lib/libdyn/include $(pwd)/lib/libsta/include $(pwd)/lib/cannyEdgeFilter/include $(pwd)/lib/rocksDbWrapper/include
} > $DOX_CONF_FILE

# Generate documentation
# dot -c clears Graphviz configuration, doxygen uses Graphviz for generating graphical representations
sudo dot -c

ERROR_FILE_FLAG=$(pwd)/dox_errors.txt

# create documentation: -s specifies comments of configurations items will be omitted.
# pipe stderr to error file
DOXYGEN_COMMAND=$(doxygen -s $DOX_CONF_FILE 2> $ERROR_FILE_FLAG)

echo "Doxygen command: $ERROR_FILE_FLAG"

# if error file not empty fail
if [ -s $ERROR_FILE_FLAG ]; then
echo "Error: There are some files that are not documented correctly"
exit 1
else
echo "All files are documented correctly. Niiiceee"
exit 0
fi

