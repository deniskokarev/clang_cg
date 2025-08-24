# Test CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Simple hello world test program
add_executable(greeting_and_farewell greeting_and_farewell.c)

# Custom target to run the callgraph analysis
add_custom_target(run_callgraph_analysis
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_callgraph_analysis.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS greeting_and_farewell YAMLCallGraphPass
        COMMENT "Running YAML CallGraph analysis on hello_world"
)
