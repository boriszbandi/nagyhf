cmake_minimum_required(VERSION 3.10)

# Set your project name
project(Borisz)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the CURL library
find_package(CURL REQUIRED)

# Include directories for your project
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Define the executable to be built from your source files
add_executable(Borisz main.cpp backend.cpp)

# Link the CURL library with your executable
target_link_libraries(Borisz ${CURL_LIBRARIES})

# Google Test configuration
# Add GoogleTest as a subdirectory. Assuming GoogleTest is cloned in the 'external' directory
add_subdirectory(external/googletest)

# Include GoogleTest directories
include_directories(external/googletest/googletest/include)
include_directories(external/googletest/googlemock/include)

# Create a test executable
add_executable(runTests test_http_response.cpp test_backend.cpp backend.cpp)

# Link the Google Test libraries with the test executable
target_link_libraries(runTests gtest gtest_main ${CURL_LIBRARIES})

# Enable testing
enable_testing()
add_test(NAME runTests COMMAND runTests)
