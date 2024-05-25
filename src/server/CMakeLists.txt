# Configure the client project
project(
  "server"
  LANGUAGES CXX 
)

# Create the executable
add_executable(${PROJECT_NAME} server.cpp server.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cppSocket/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cannyEdgeFilter/include)

# Link libraries 
target_link_libraries(${PROJECT_NAME}
    SocketWrapper
    # rocksdb
    nlohmann_json::nlohmann_json
    rocksDBWrapper
    cannyEdge
    zip
    OpenMP::OpenMP_CXX
    DynamiCLib
    StatiCLib
)
