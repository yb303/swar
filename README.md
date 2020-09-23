# SWAR
Low level, branch-free functions for number and string conversion and other utils.

SWAR stands for SIMD within a register. This means treating parts of a uint64_t as individual uint8_t's, uint16_t's, or uint32_t's.

Most functions have different variants optimized for limited lengths (8 and 4), and printable input (ascii < 128), and more

### Performance
Performace of atoi implementations (actually a-to-ull)
The limiting factor here is the multiplication. You can see how the naive impl is climbing at fixed rate as it multiplies once per digit, while the SWAR is doing less multiplications.
The stock atoll is teribly slow and I did not yet read the code to see why.

<img src="https://lh3.googleusercontent.com/KeHaQM5RM2_hS6Gf8MEtRlV4EgVwnJBqLnxMinczB67XUaR8wXyriYNrqY4ukYBE0aGNQ4TDA31f1MIP57j4r78zmeYk3OAqFySu2yfHQClDhxMnm86ACtUVI6EQSGX7aF0HgTYD3Cg=w2400">

And zoomed into small numbers

<img src="https://lh3.googleusercontent.com/QG7M04TQueHiBw9WJcpYyNCZHpcc2DT3LdUL5EmCX_wJZqZlHrbTrIVcuWkzbxGrM73bZed_hJh_ocGVgOTiUFJXwCaFG-4FJQXFsxG6MB7bw79MzYZi6N-FHU_xo1ZCHSMBAFW-DHM=w2400">



### Functions
* memchr
* strlen
* atoi, itoa, atod

### Supported operating systems
* Linux
* Cygwin

### Supported programming languages
* C++

