cmake_minimum_required(VERSION 3.20)
project(llvm_pass_cg VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Function to find LLVM on Mac with Homebrew
function(find_brew_llvm)
    if (NOT LLVM_FOUND AND APPLE)
        message(STATUS "LLVM not found in default paths, trying macOS-brew...")

        # Try to find LLVM via Homebrew
        execute_process(
                COMMAND brew --prefix llvm
                OUTPUT_VARIABLE BREW_LLVM_PREFIX
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
        )

        if (BREW_LLVM_PREFIX)
            set(LLVM_DIR "${BREW_LLVM_PREFIX}/lib/cmake/llvm" PARENT_SCOPE)
            message(STATUS "Trying Homebrew LLVM path: ${BREW_LLVM_PREFIX}/lib/cmake/llvm")
        endif ()
    endif ()
endfunction()

# Try to find LLVM first with default paths
find_package(LLVM CONFIG QUIET)
# If prior find failed try to find LLVM in homebrew
find_brew_llvm()

# Final attempt
if (NOT LLVM_FOUND)
    find_package(LLVM REQUIRED CONFIG)
endif ()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM definitions and include directories
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Link against LLVM libraries
llvm_map_components_to_libnames(llvm_libs
        Passes
)

# Now build our plugin
add_library(YAMLCallGraphPass SHARED
        YAMLCallGraphPass.cpp)

# Link against LLVM
target_link_libraries(YAMLCallGraphPass ${llvm_libs})

# LLVM is (typically) built with no C++ RTTI. We need to match that.
if (NOT LLVM_ENABLE_RTTI)
    target_compile_options(YAMLCallGraphPass PRIVATE -fno-rtti)
endif ()

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if (APPLE)
    set_target_properties(YAMLCallGraphPass PROPERTIES
            LINK_FLAGS "-undefined dynamic_lookup"
    )
endif (APPLE)

# Add test subdirectory
add_subdirectory(test)
