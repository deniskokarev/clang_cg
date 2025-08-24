# LLVM Pass for Call Graph Analysis

This is an example of how to implement an LLVM pass that generates YAML-formatted call
graphs for C/C++ application.

Verified with homebrew clang 20 on Mac and clang 18 on Ubuntu 24.04

It comes down to executing a custom llvm pass on intermediate llvm bitcode.
If you had an intermediate bitcode produced via
```shell
clang -S -emit-llvm your.c -o your.ll
```
Then
```shell
opt -load-pass-plugin=libYAMLCallGraphPass.dylib -passes="yaml-callgraph" -yaml-callgraph-output="your.callgraph.yaml" your.ll
```
will produce the callgraph for it.

If you want to build a callgraph for many modules or entire application, then `llvm-link` step has to be used to
link all *.ll bitcode modules into one large *.ll file.


# Requirements

### Mac

Install XCode with command line tools and `brew`, then
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
