#!/bin/bash

set -e

# Plugin path is passed as first argument from CMake
PLUGIN_LIB="${1:?'Plugin path not provided as argument"'}"
SRC="${2:?'Src file must be given'}"

echo "Using plugin: $PLUGIN_LIB"

# Compile C source to LLVM IR
echo "Step 1: Compiling greeting_and_farewell.c to LLVM IR..."
clang -S -emit-llvm -o greeting_and_farewell.ll "${SRC}"

# Run our YAML CallGraph pass
echo "Step 2: Running YAML CallGraph pass..."
opt -load-pass-plugin="$PLUGIN_LIB" -passes="yaml-callgraph" -yaml-callgraph-output="callgraph.yaml" greeting_and_farewell.ll -o /dev/null

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