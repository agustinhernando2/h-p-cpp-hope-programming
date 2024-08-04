# Configure the client project
project(client)
# Flags de compilación
set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -Wconversion -fprofile-arcs -ftest-coverage")

# Create the executable
add_executable(${PROJECT_NAME} client.cpp)

# Include directories for libraries and cJSON
# target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libdyn/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libsta/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cppSocket/include)

# Link libraries 
target_link_libraries(${PROJECT_NAME}
    SocketWrapper
    # cJSON
    nlohmann_json::nlohmann_json
    # rocksDBWrapper
    # cannyEdge
    ZLIB::ZLIB
    # OpenMP::OpenMP_CXX
    # DynamiCLib
    StatiCLib
)
