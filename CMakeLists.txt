cmake_minimum_required(VERSION 3.10)

# Set your project name
project(MyProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the CURL library
find_package(CURL REQUIRED)

# Include directories for your project
include_directories(${CURL_INCLUDE_DIR})
# If you've placed the json.hpp in a specific include directory, specify it here
include_directories(${PROJECT_SOURCE_DIR}/include)

# Define the executable to be built from your source files
add_executable(MyExecutable backend.cpp main.cpp) # Include main.cpp here

# Link the CURL library with your executable
target_link_libraries(MyExecutable ${CURL_LIBRARIES})

# If you're using a specific path for the nlohmann/json.hpp and it's not in a standard include path, you might need to adjust
# the include_directories command above to point to the correct directory where json.hpp is located.