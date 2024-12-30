cmake_minimum_required(VERSION 3.5)
project(k_13c)

# CMake standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LexicalAnalyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SyntaxAnalyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SemanticAnalyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Generator.cpp
    )

# Create executable
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set the destination directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

