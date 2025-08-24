# Test CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# not actually used
add_executable(greeting_and_farewell greeting_and_farewell.c)

# Target to generate LLVM IR from C source
add_custom_command(
    OUTPUT greeting_and_farewell.ll
    COMMAND "${LLVM_BINARY_DIR}/bin/clang" -S -emit-llvm -o greeting_and_farewell.ll "${CMAKE_CURRENT_SOURCE_DIR}/greeting_and_farewell.c"
    DEPENDS greeting_and_farewell.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating LLVM IR from greeting_and_farewell.c"
)

# Target to generate YAML callgraph from LLVM IR
add_custom_command(
    OUTPUT greeting_and_farewell.callgraph.yaml
    COMMAND "${LLVM_BINARY_DIR}/bin/opt" -load-pass-plugin="$<TARGET_FILE:YAMLCallGraphPass>" -passes="yaml-callgraph" -yaml-callgraph-output="greeting_and_farewell.callgraph.yaml" greeting_and_farewell.ll -o /dev/null
    DEPENDS greeting_and_farewell.ll YAMLCallGraphPass
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating YAML callgraph from greeting_and_farewell.ll"
)

# Custom targets to make the files buildable
add_custom_target(greeting_and_farewell_ll
    DEPENDS greeting_and_farewell.ll
)

add_custom_target(check_callgraph_yaml
    DEPENDS greeting_and_farewell.callgraph.yaml
    COMMAND echo "YAML callgraph generated at: ${CMAKE_CURRENT_BINARY_DIR}/greeting_and_farewell.callgraph.yaml"
)
