cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
  "HPCPP"
  VERSION 1.0.0
  DESCRIPTION "High Performance C++"
  LANGUAGES CXX C
)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

include(FetchContent)

set(GTEST_GIT_URL "https://github.com/google/googletest.git")
set(ROCKSDB_GIT_URL "https://github.com/facebook/rocksdb.git")
set(NLOHMANN_JSON_GIT_URL "https://github.com/nlohmann/json.git")
set(CPP_HTTPLIB_GIT_URL "https://github.com/yhirose/cpp-httplib.git")
set(CJSON_GIT_URL "https://github.com/DaveGamble/cJSON.git")

# # to avoid building unnecessary targets
# set(WITH_BENCHMARK_TOOLS OFF)
# set(WITH_TOOLS OFF)
# set(WITH_TESTS OFF)
# set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY ${GTEST_GIT_URL}
  GIT_TAG release-1.12.0  # Optionally pin to a stable release
)

FetchContent_Declare(
  rocksdb
  GIT_REPOSITORY ${ROCKSDB_GIT_URL}
  GIT_TAG v9.0.0
)

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

FetchContent_Declare(
  cJSON
  GIT_REPOSITORY ${CJSON_GIT_URL}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/cJSON
  UPDATE_COMMAND ""
)

# FetchContent_MakeAvailable(cJSON)
# FetchContent_MakeAvailable(googletest)
# FetchContent_MakeAvailable(rocksdb)
# FetchContent_MakeAvailable(nlohmann_json)
# FetchContent_MakeAvailable(cpp_httplib)



# Compile the libraries
add_subdirectory(lib/libdyn)
add_subdirectory(lib/libsta)
add_subdirectory(src/client)
add_subdirectory(src/server)
add_subdirectory(external)
add_subdirectory(lib/cppSocket)




if (RUN_TESTS EQUAL 1 OR RUN_COVERAGE EQUAL 1)
    add_subdirectory(tests)

    # Link Google Test to the test executable
    add_executable(unit_tests ${TEST_SOURCES})
    target_include_directories(unit_tests PUBLIC ${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include)
    target_link_libraries(unit_tests gtest_main gmock_main)

    # Add the tests to be run by CTest
    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif()
