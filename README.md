# SWAR
Low level, branch-free functions for number and string conversion and other utils.

SWAR stands for SIMD within a register. This means treating parts of a uint64_t as individual uint8_t's, uint16_t's, or uint32_t's.

This library is header-only. There is nothing to build.

Most functions have different variants optimized for limited lengths (8 and 4), and printable input (ascii < 128), and more

### Language and build

I'm used to C++17 so used `if constexpr`, but the rest of the code is C++03 copatible, and can easily be converted to C.<br>
Include `swar.h` and build with -std=c++17<br>
For forward declarations only, include `swar_fwd.h` instead.

### Test and benchmark

The test dir includes:
- `swar_test.cpp` that is using google-test for unit testing. (**TODO** create a `build: passing` badge)<br>
- `swar_bench.cpp` that produces the numbers for the graph below.<br>

### Performance

Comparison of various atoi implementations (actually a-to-ull) on my home machine:
<img src="https://lh3.googleusercontent.com/KeHaQM5RM2_hS6Gf8MEtRlV4EgVwnJBqLnxMinczB67XUaR8wXyriYNrqY4ukYBE0aGNQ4TDA31f1MIP57j4r78zmeYk3OAqFySu2yfHQClDhxMnm86ACtUVI6EQSGX7aF0HgTYD3Cg=w2400"><br>
Machine and env spec: i5-3470, Cygwin on Windows 10 64 bit, g++ 9.3.0

Functions with 8, or 4, suffix are branchless. Functions with longer input must have a branch per word.<br>
An SSE implementation can follow the same ideas as here for longer inputs. However, using SSE instruction may switch some processors to a different P-state, if the BIOS allows, and the switching itself can take a few hundred cycles.

Branchless code is not always faster than branched code.<br>
Benchmarks are typically less impacted by branch miss-predictions, then real world applications. This applies also in my benchmark. I did not take special care to litter the BP caches before each function call as this would make each call harder to measure.<br>
Loops may be fully predicted, especially if BP caches are all working for the benchmark. However, in a real world app, using branchless low level code means that BP caches have more room for the application logic so the app as a whole may become faster. The only way to know for sure is to test within the app.

These performance characteristics are the same for strlen and similar functions.

### Functions
* memchr
* strlen
* atoi, itoa, atod
* 

### Supported operating systems
* Linux
* Cygwin

### Supported programming languages
* C++

### Supported compilers
* g++

### Supported architectures
* x86_64

