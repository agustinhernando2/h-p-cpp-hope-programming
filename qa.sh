# Report of Ctest

echo "=== Runing Ctest ==="

CURRENT_PATH=$(pwd)

TEST_DIR="./build/tests"

cd ${TEST_DIR}

ctest --output-junit ctest_report.xml

echo "=== Runing Valgrind ==="

valgrind --leak-check=full --xml=yes --xml-file=valgrind_report.xml ctest

echo "Report of Valgrind is generated in ${TEST_DIR}/valgrind_report.xml"

cd ..

echo "=== Runing Scan build ==="


scan-build -plist -analyze-headers -o analyzer_reports ninja

echo "Report of Scan build is generated in ./analyzer_reports"

cd ${CURRENT_PATH}

echo "Report of Ctest is generated in ${TEST_DIR}/ctest_report.xml"

echo "=== Runing Cppcheck ==="

SRC=$(find ./src/ -type f \( -name "*.c" \) | tr '\n' ' ')


cppcheck -v --enable=all --xml -I include, lib/libdyn/include, lib/libsta/include ${SRC} 2> ./build/tests/cpp_report.xml

echo "Report of Cppcheck is generated in ./build/tests/cpp_report.xml"
