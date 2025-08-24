# LLVM Pass for Call Graph Analysis

This is an example of how to implement an LLVM pass that generates YAML-formatted call
graphs from C/C++ source code.

# Requirements

### Mac

```shell
brew install cmake llvm
```

### Ubuntu

```shell
sudo apt install cmake build-essential libzstd-dev llvm clang
```

# Build / Test

Make sure you have `clang` installed. If you're on Mac, the XCode will not contain all
necessary libraries and using `brew install llvm` is recommended.

**cmake configure**
```shell
cmake -B build -S .
```

**build/test**
```shell
cmake --build build -t check_callgraph_yaml
```

Expected Output:
```yaml
callgraph:
  functions:
    - id: "func_0x600002774008"
      name: "print_greeting"
      children:
        - "func_0x600002774128"
    - id: "func_0x6000027741b8"
      name: "print_farewell"
      children:
        - "func_0x600002774128"
        - "func_0x6000027741b8"
    - id: "func_0x600002774368"
      name: "main"
      children:
        - "func_0x600002774008"
        - "func_0x6000027741b8"
```
