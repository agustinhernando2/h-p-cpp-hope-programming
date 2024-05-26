cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
  "HPCPP"
  VERSION 1.0.0
  DESCRIPTION "High Performance C++"
  LANGUAGES CXX C
)

# Flags de compilación
set(CMAKE_C_FLAGS "-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11")

# Opciones de configuración
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(WITH_BENCHMARK_TOOLS OFF)
set(WITH_TOOLS OFF)
set(WITH_TESTS OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

include(FetchContent)

set(NLOHMANN_JSON_GIT_URL "https://github.com/nlohmann/json.git")
set(CPP_HTTPLIB_GIT_URL "https://github.com/yhirose/cpp-httplib.git")
set(OPENCV_GIT_URL "https://github.com/opencv/opencv.git")
set(GDAL_GIT_URL "https://github.com/OSGeo/gdal.git")
set(ROCKSDB_GIT_URL "https://github.com/facebook/rocksdb.git")
set(CJSON_GIT_URL "https://github.com/DaveGamble/cJSON.git")

# Fetch dependencias
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY ${NLOHMANN_JSON_GIT_URL}
  GIT_TAG v3.11.2
)

FetchContent_Declare(
  cpp_httplib
  GIT_REPOSITORY ${CPP_HTTPLIB_GIT_URL}
  GIT_TAG v0.15.3
)

# Include Google Test and Google Mock
set(GTEST_GIT_URL "https://github.com/google/googletest.git")
FetchContent_Declare(
  googletest
  GIT_REPOSITORY ${GTEST_GIT_URL}
  GIT_TAG release-1.12.0  
)

FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(nlohmann_json)
FetchContent_MakeAvailable(cpp_httplib)

# CJSON
FetchContent_Declare(
  cJSON
  GIT_REPOSITORY ${CJSON_GIT_URL}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/cJSON
  UPDATE_COMMAND ""
)
FetchContent_MakeAvailable(cJSON)

# Agregar el path a gflags
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/gflags" ${CMAKE_PREFIX_PATH})

#OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(OPENMP_LIB OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "OpenMP not found")
endif()

#ZLIB
find_package(ZLIB)
if(ZLIB_FOUND)
    set(ZLIB_LIB ZLIB::ZLIB)
else()
    message(FATAL_ERROR "ZLIB not found")
endif()

#OpenCV
find_package(OpenCV)
if(OpenCV_FOUND)
    set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
else()
  message(STATUS "OpenCV not found in the system, fetching from GitHub")
  FetchContent_Declare(
  opencv
  GIT_REPOSITORY ${OPENCV_GIT_URL}
  GIT_TAG 4.5.3
  )
  FetchContent_MakeAvailable(opencv)
  set(OpenCV_DIR ${opencv_SOURCE_DIR})
  find_package(OpenCV REQUIRED)
endif()
include_directories(${OpenCV_INCLUDE_DIRS})

#GDAL
find_package(GDAL)
if (GDAL_FOUND)
    message(STATUS "GDAL found in the system")
    set(GDAL_LIB GDAL::GDAL)
    include_directories(${GDAL_INCLUDE_DIRS})
else()
    message(STATUS "GDAL not found in the system, fetching from GitHub")
    FetchContent_Declare(
    gdal
    GIT_REPOSITORY ${GDAL_GIT_URL}
    GIT_TAG v3.3.0
    )
    FetchContent_MakeAvailable(gdal)
    set(GDAL_DIR ${gdal_SOURCE_DIR})
endif()

# cpp_httplib
if (cpp_httplib_POPULATED)
    include_directories(${cpp_httplib_SOURCE_DIR})
else()
    message(FATAL_ERROR "cpp_httplib not found")
endif()

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/lib/rocksDbWrapper/include) 

# Compile the libraries
add_subdirectory(lib/rocksDbWrapper)
add_subdirectory(lib/cannyEdgeFilter)
add_subdirectory(lib/libdyn)
add_subdirectory(lib/libsta)
add_subdirectory(src/client)
add_subdirectory(src/server)
add_subdirectory(external)
add_subdirectory(lib/cppSocket)

if (RUN_TESTS EQUAL 1 OR RUN_COVERAGE EQUAL 1)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
