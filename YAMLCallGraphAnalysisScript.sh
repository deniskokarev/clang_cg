#!/bin/bash

set -e

echo "=== Building hello_world and running YAML CallGraph analysis ==="

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
BUILD_DIR="$(pwd)"

# Find the plugin library
PLUGIN_LIB=""
if [ -f "../libYAMLCallGraphPass.so" ]; then
    PLUGIN_LIB="../libYAMLCallGraphPass.so"
elif [ -f "../libYAMLCallGraphPass.dylib" ]; then
    PLUGIN_LIB="../libYAMLCallGraphPass.dylib"
else
    echo "Error: Could not find YAMLCallGraphPass plugin library"
    exit 1
fi

echo "Using plugin: $PLUGIN_LIB"

# Compile C source to LLVM IR
echo "Step 1: Compiling hello_world.c to LLVM IR..."
clang -S -emit-llvm -o hello_world.ll "${SCRIPT_DIR}/hello_world.c"

# Run our YAML CallGraph pass
echo "Step 2: Running YAML CallGraph pass..."
opt -load-pass-plugin="$PLUGIN_LIB" -passes="yaml-callgraph" -yaml-callgraph-output="callgraph.yaml" hello_world.ll -o /dev/null

# Display the results
echo "Step 3: Generated YAML CallGraph:"
echo "=========================="
if [ -f "callgraph.yaml" ]; then
    cat callgraph.yaml
    echo "=========================="
    echo "Analysis complete! YAML callgraph saved to: callgraph.yaml"
else
    echo "Error: callgraph.yaml was not generated"
    exit 1
fi

# Also compile and run the program for verification
echo "Step 4: Building and running hello_world executable..."
clang -o hello_world "${SCRIPT_DIR}/hello_world.c"
echo "Program output:"
./hello_world