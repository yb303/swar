# SWAR
Low level, branch-free functions for number and string conversion and other utils.

SWAR stands for SIMD within a register. This means treating parts of a uint64_t as individual uint8_t's, uint16_t's, or uint32_t's.

Most functions have different variants optimized for limited lengths (8 and 4), and printable input (ascii < 128), and more

### Performance
Performace of atoi implementations (actually a-to-ull)
The limiting factor here is the multiplication. You can see how the naive impl is climbing at fixed rate as it multiplies once per digit, while the SWAR is doing less multiplications.
The stock atoll is teribly slow and I did not yet read the code to see why.

<img src="https://drive.google.com/file/d/1XhfXByPBCuf673qrZriowbQhHNJnQKOv/preview" title="atoi performance" alt="atoi performance">

And zoomed into small numbers

<img src="https://drive.google.com/file/d/1AbL6g9tPEcCTV4pHR4R-LT-E8eWHJ8Mn/preview" title="atoi performance" alt="atoi performance">



### Functions
* memchr
* strlen
* atoi, itoa, atod

### Supported operating systems
* Linux
* Cygwin

### Supported programming languages
* C++

