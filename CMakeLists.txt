cmake_minimum_required(VERSION 3.20)
project(llvm_pass_cg VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM installation
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Now build our plugin
add_library(YAMLCallGraphPass SHARED
    YAMLCallGraphPass.cpp)

# Use C++17 to compile our pass (std::experimental::filesystem)
target_compile_features(YAMLCallGraphPass PRIVATE cxx_std_17)

# LLVM is (typically) built with no C++ RTTI. We need to match that.
if (NOT LLVM_ENABLE_RTTI)
  target_compile_options(YAMLCallGraphPass PRIVATE -fno-rtti)
endif()

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(YAMLCallGraphPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)

# Add test subdirectory
add_subdirectory(test)