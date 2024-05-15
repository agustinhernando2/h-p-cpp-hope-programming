# Configure the client project
project(
  "server"
  LANGUAGES CXX 
)

# Add header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Create the executable
add_executable(${PROJECT_NAME} server.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cppSocket/include)

# Link libraries 
target_link_libraries(${PROJECT_NAME} PUBLIC SocketWrapper) 

#OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "OpenMP not found")
endif()

