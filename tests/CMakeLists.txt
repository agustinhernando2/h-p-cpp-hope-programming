cmake_minimum_required(VERSION 3.25 FATAL_ERROR)


# Incluye los directorios de los archivos de encabezado
include_directories(include)
# include_directories(${CMAKE_SOURCE_DIR}/lib/cppSocket/include)
# include_directories(${CMAKE_SOURCE_DIR}/lib/cannyEdgeFilter/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/libsta/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/libdyn/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/rocksDbWrapper/include)

# Incluye los directorios de las bibliotecas de Google Test
include_directories(${googletest_SOURCE_DIR}/googletest/include)
include_directories(${googletest_SOURCE_DIR}/googlemock/include)

# Agrega un ejecutable para las pruebas
file(GLOB_RECURSE TEST_SOURCES 
${CMAKE_SOURCE_DIR}/tests/unit/unit_tests.cpp
)

if(RUNS_COVERAGE)
	message("Coverage enabled")
	find_program(LCOV_PATH lcov)
		if(NOT LCOV_PATH)
		message(FATAL_ERROR "lcov not found! Aborting..")
	endif()

	find_program(GCOV_PATH gcov)
		if(NOT GCOV_PATH)
		message(FATAL_ERROR "gcov not found! Aborting..")
	endif()

	find_program(GENHTML_PATH genhtml)
		if(NOT GENHTML_PATH)
		message(FATAL_ERROR "genh tml not found! Aborting..")
	endif()
	# Crear el directorio de reports si no existe
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/reports)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage --coverage")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
    endif()

# Create test executable
add_executable(test_${PROJECT_NAME} ${TEST_SOURCES} )

# Link with Unity
target_link_libraries(test_${PROJECT_NAME} 
    gtest
    gtest_main
    nlohmann_json::nlohmann_json
	SocketWrapper
    rocksDBWrapper
    cannyEdge
    ZLIB::ZLIB
    OpenMP::OpenMP_CXX
    DynamiCLib
    StatiCLib
)

# Add test
add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})