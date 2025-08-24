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


# CFG analysis

TBD, but to experiment you can inspect some functions using existing tools:
```shell
% cd test
% opt -passes="dot-cfg" greeting_and_farewell.ll -o /dev/null
Writing '.print_greeting.dot'...
Writing '.print_farewell.dot'...
Writing '.main.dot'...
```

Open produced Graphviz `dot` file locally, like so
```shell
dot -Tsvg .print_farewell.dot -o print_farewell.svg
open print_farewell.svg
```
or you can use some [Online Viewer](https://dreampuf.github.io/GraphvizOnline/?engine=dot#digraph%20%22CFG%20for%20'print_farewell'%20function%22%20%7B%0A%09label%3D%22CFG%20for%20'print_farewell'%20function%22%3B%0A%0A%09Node0x60000027caf0%20%5Bshape%3Drecord%2Ccolor%3D%22%23b70d28ff%22%2C%20style%3Dfilled%2C%20fillcolor%3D%22%23b70d2870%22%20fontname%3D%22Courier%22%2Clabel%3D%22%7B1%3A%5Cl%7C%20%20%252%20%3D%20alloca%20ptr%2C%20align%208%5Cl%20%20store%20ptr%20%250%2C%20ptr%20%252%2C%20align%208%5Cl%20%20%253%20%3D%20load%20ptr%2C%20ptr%20%252%2C%20align%208%5Cl%20%20%254%20%3D%20call%20i32%20(ptr%2C%20...)%20%40printf(ptr%20noundef%20%40.str.1%2C%20ptr%20noundef%20%253)%5Cl%20%20%255%20%3D%20load%20ptr%2C%20ptr%20%252%2C%20align%208%5Cl%20%20%256%20%3D%20getelementptr%20inbounds%20i8%2C%20ptr%20%255%2C%20i64%200%5Cl%20%20%257%20%3D%20load%20i8%2C%20ptr%20%256%2C%20align%201%5Cl%20%20%258%20%3D%20sext%20i8%20%257%20to%20i32%5Cl%20%20%259%20%3D%20icmp%20eq%20i32%20%258%2C%2087%5Cl%20%20br%20i1%20%259%2C%20label%20%2510%2C%20label%20%2511%5Cl%7C%7B%3Cs0%3ET%7C%3Cs1%3EF%7D%7D%22%5D%3B%0A%09Node0x60000027caf0%3As0%20-%3E%20Node0x60000027cb40%3B%0A%09Node0x60000027caf0%3As1%20-%3E%20Node0x60000027cb90%3B%0A%09Node0x60000027cb40%20%5Bshape%3Drecord%2Ccolor%3D%22%233d50c3ff%22%2C%20style%3Dfilled%2C%20fillcolor%3D%22%23be242e70%22%20fontname%3D%22Courier%22%2Clabel%3D%22%7B10%3A%5Cl%7C%20%20call%20void%20%40print_farewell(ptr%20noundef%20%40.str.2)%5Cl%20%20br%20label%20%2511%5Cl%7D%22%5D%3B%0A%09Node0x60000027cb40%20-%3E%20Node0x60000027cb90%3B%0A%09Node0x60000027cb90%20%5Bshape%3Drecord%2Ccolor%3D%22%23b70d28ff%22%2C%20style%3Dfilled%2C%20fillcolor%3D%22%23b70d2870%22%20fontname%3D%22Courier%22%2Clabel%3D%22%7B11%3A%5Cl%7C%20%20ret%20void%5Cl%7D%22%5D%3B%0A%7D%0A)