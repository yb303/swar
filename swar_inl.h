#pragma once

#include "compiler.h"

#include <assert.h>
#include <string.h> // for memcpy, memset
#include <stdint.h>

// Function naming convention [prefix] <function> [length]
// - function

namespace swar {

// cast and fill utils
template <typename T>
inline T cast(const char* src) {
    T ret;
    ::memcpy(&ret, src, sizeof(T));
    return ret;
}

template <typename T>
inline T extend(char c) {
    T ret;
    ::memset(&ret, c, sizeof(T));
    return ret;
}

// swap bytes
inline uint64_t bswap(uint64_t x) { return __builtin_bswap64(x); }
inline uint32_t bswap(uint32_t x) { return __builtin_bswap32(x); }
inline uint16_t bswap(uint16_t x) { return __builtin_bswap16(x); }

//// Find bytes

inline bool haszero(uint64_t x) {
    uint64_t a = 0x7f7f7f7f7f7f7f7full;
    uint64_t l = 0x0101010101010101ull;
    return (x - l) & ~x & ~a;
}

// Check if word has some byte
inline bool hasbyte(uint64_t x, uint8_t c) {
    return haszero(x ^ extend<uint64_t>(c));
}

// Find char in string. Support all options.
template<bool Printable, bool Exists, bool Reverse=false>
inline uint32_t _memchr8(const char* s, uint8_t c) {
    // int 64 of all c's
    uint64_t m = extend<uint64_t>(c);

    // int 64 of s
    uint64_t x = cast<uint64_t>(s);

    // remove c's from string
    // so now we have to find first zero byte
    x ^= m;

    uint64_t a = 0x7f7f7f7f7f7f7f7full;

    // set the high bit in non-zero bytes
    if (Printable) {
        x += a;
    }
    else {
        x = ((x & a) + a) | x;
    }

    // flip to set the high bit in zero bytes, and clear other high bits
    x = ~x;

    // clear all bits except the high bit of the zero byte
    x &= ~a;

    // find the high bit, from right (little endian)
    if (Exists) {
        if (!Reverse) {
            // ctz returns 7, 15, 23, etc
            return __builtin_ctzll(x) / 8;
        }
        else {
            // clz returns 0, 8, 16, etc
            return 7 - __builtin_clzll(x) / 8;
        }
    }
    else {
        if (!Reverse) {
            // ffs returns + 1, so that's going to be 8, 16, 24, etc
            return (__builtin_ffsll(x) - 8) / 8;
        }
        else {
            // clz returns 0, 8, 16, 24, 32, 40, 48, 56
            // x == 0 returns 63 that we increment to 64 to return -1
            return 7 - (__builtin_clzll(x | 1) + 1) / 8;
        }
    }
}

// Find char in string and trim it
template<bool Printable, bool Exists>
inline uint32_t _trim8(const char* s, uint8_t c) {
    // int 64 of all c's
    uint64_t m = extend<uint64_t>(c);

    // int 64 of s
    uint64_t x = cast<uint64_t>(s);
    uint64_t xo = x;

    // remove c's from string
    // so now we have to find first zero byte
    x ^= m;

    uint64_t a = 0x7f7f7f7f7f7f7f7full;

    // set the high bit in non-zero bytes
    if (Printable) {
        x += a;
    }
    else {
        x = ((x & a) + a) | x;
    }

    // flip to set the high bit in zero bytes, and clear other high bits
    x = ~x;

    // clear all bits except the high bit of the zero byte
    x &= ~a;

    // set all bits under the lowest high bit
    x &= x - 1u;

    if (!Exists) {
        x >>= 7;
    }
    else if (Printable) {
        // complete the killed high bit
        x <<= 1u;
        x |= 1u;
    }

    return xo & x;
}

// Find char in printable (chars < 128) string of 8 chars
inline uint32_t pmemchr8(const char* s, uint8_t c) {
    return _memchr8<true, false>(s, c);
}

// Find char in printable (chars < 128) string of 8 chars
// * The string is known to contain the char
inline uint32_t pmemchr8k(const char* s, uint8_t c) {
    return _memchr8<true, true>(s, c);
}

// Find char in binary string of 8 chars
inline uint32_t memchr8(const char* s, uint8_t c) {
    return _memchr8<false, false>(s, c);
}

// Find char in binary string of 8 chars
// * The string is known to contain the char
inline uint32_t memchr8k(const char* s, uint8_t c) {
    return _memchr8<false, true>(s, c);
}

// Find char in printable (chars < 128) string of 8 chars
inline uint32_t pmemrchr8(const char* s, uint8_t c) {
    return _memchr8<true, false, true>(s, c);
}

// Find char in printable (chars < 128) string of 8 chars
// * The string is known to contain the char
inline uint32_t pmemrchr8k(const char* s, uint8_t c) {
    return _memchr8<true, true, true>(s, c);
}

// Find char in binary string of 8 chars
inline uint32_t memrchr8(const char* s, uint8_t c) {
    return _memchr8<false, false, true>(s, c);
}

// Find char in binary string of 8 chars
// * The string is known to contain the char
inline uint32_t memrchr8k(const char* s, uint8_t c) {
    return _memchr8<false, true, true>(s, c);
}

// Find char in const binary string
template<bool Printable, bool Known>
inline uint32_t _memchr(const char* s, uint32_t len, uint8_t c) {
    const char* p = s;
    const char* end = s + len;

    // If shorter than 8 bytes, we have to mask away c bytes past len
    uint32_t partLen = (len & 7) ? (len & 7) : 8;
    uint64_t partMask = len < 8 ? ~0ull >> (64 - partLen * 8) : 0ull;
    uint64_t first = cast<uint64_t>(p) & ~(partMask & extend<uint64_t>(c));

    // Check first 8 bytes
    if (hasbyte(first, c))
        return _memchr8<Printable, true>((char*)&first, c);
    
    // Advance to leave multiple of 8 bytes
    p += partLen;

    // Check words for that byte
    for (;;) {
        if (hasbyte(cast<uint64_t>(p), c)) {
            return (p - s) + _memchr8<Printable, true>(p, c);
        }
        p += 8;
        if (!Known) {
            if (p == end)
                return -1;
        }
    }
}

// Find char, in reverse, in const binary string
template<bool Printable, bool Known>
inline uint32_t _memrchr(const char* s, uint32_t len, uint8_t c) {
    const char* p = s + len;

    // If shorter than 8 bytes, we have to mask away c bytes past len
    uint32_t partLen = (len & 7) ? (len & 7) : 8;
    uint64_t partMask = len < 8 ? ~0ull >> (64 - partLen * 8) : 0ull;
    uint64_t first = cast<uint64_t>(p) & ~(partMask & extend<uint64_t>(c));

    // Advance to leave multiple of 8 bytes
    p -= partLen;

    // Check first 8 bytes
    if (hasbyte(first, c)) {
        return (p - s) + _memchr8<Printable, true, true>((char*)&first, c);
    }
    
    // Check words for that byte
    for (;;) {
        if (hasbyte(cast<uint64_t>(p), c)) {
            return (p - s) + _memchr8<Printable, true, true>(p, c);
        }
        p -= 8;
        if (!Known) {
            if (p == s)
                return -1;
        }
    }
}

// Find char in binary string
inline uint32_t memchr(const char* s, uint32_t len, uint8_t c) {
    return _memchr<false, false>(s, len, c);
}

// Find char in binary string. Char c is known to be in s + len
inline uint32_t memchrk(const char* s, uint32_t len, uint8_t c) {
    return _memchr<false, true>(s, len, c);
}

// Find char in printable string
inline uint32_t pmemchr(const char* s, uint32_t len, uint8_t c) {
    return _memchr<true, false>(s, len, c);
}

// Find char in printable string. Char c is known to be in s + len
inline uint32_t pmemchrk(const char* s, uint32_t len, uint8_t c) {
    return _memchr<true, true>(s, len, c);
}

// Find char in binary string
inline uint32_t memrchr(const char* s, uint32_t len, uint8_t c) {
    return _memrchr<false, false>(s, len, c);
}

// Find char in binary string. Char c is known to be in s + len
inline uint32_t memrchrk(const char* s, uint32_t len, uint8_t c) {
    return _memrchr<false, true>(s, len, c);
}

// Find char in printable string
inline uint32_t pmemrchr(const char* s, uint32_t len, uint8_t c) {
    return _memrchr<true, false>(s, len, c);
}

// Find char in printable string. Char c is known to be in s + len
inline uint32_t pmemrchrk(const char* s, uint32_t len, uint8_t c) {
    return _memrchr<true, true>(s, len, c);
}

// Find char in NON-CONST string
template<bool Printable>
inline uint32_t _memchr_nc(char* s, uint32_t len, uint8_t c) {
    const char* p = s;

    // Replace back with sentinel so we don't have to check for length
    uint8_t back = s[len - 1];
    s[len - 1] = c;

    // Check words for that byte.
    while (!hasbyte(cast<uint64_t>(p), c)) {
        p += 8;
    }

    // Find the position
    p += _memchr8<Printable, true>(p, c);

    // restore sentinel
    s[len - 1] = back;

    uint32_t ret = p - s;
    return (ret != len - 1 || back == c) ? ret : -1;
}

// Find char in binary NON-CONST string
inline uint32_t memchr_nc(char* s, uint32_t len, uint8_t c) {
    return _memchr_nc<false>(s, len, c);
}

// Find char in printable NON-CONST string
inline uint32_t pmemchr_nc(char* s, uint32_t len, uint8_t c) {
    return _memchr_nc<true>(s, len, c);
}

// Find zero byte in binary string up to 8 chars
inline uint32_t strlen8(const char* s) {
    return memchr8(s, 0);
}

// Find zero byte in printable string up to 8 chars
inline uint32_t pstrlen8(const char* s) {
    return pmemchr8(s, 0);
}

// Find zero byte in binary string
inline uint32_t strlen(const char* s) {
    // check words for zero
    const char* p = s;
    while (!haszero(cast<uint64_t>(p))) {
        p += 8;
    }

    return p - s + memchr8k(p, 8);
}

// Find zero byte in printable string
inline uint32_t pstrlen(const char* s) {
    // check words for zero
    const char* p = s;
    while (!haszero(cast<uint64_t>(p))) {
        p += 8;
    }

    return p - s + pmemchr8k(p, 8);
}

// Get the uint _cast_ of a string of up to 8 chars
inline uint64_t cast8(const char* s, uint32_t len) {
    assert(len <= 8);

    // int 64 of s
    uint64_t x = cast<uint64_t>(s);

    uint64_t mask = (1ull << (len * 8)) - 1;
    mask |= -(len == 8); // fill 1's if len == 8

    return x & mask;
}

//// string to int

// Parse uint from string of up to 4 chars
inline uint16_t atou4(const char* s, uint32_t len) {
    assert(len <= 4);

    // int 64 of s. "1234" --> 0x34333231
    uint32_t x = cast<uint32_t>(s);

    // apply len. len of 2 --> 0x32310000
    x <<= 32 - len * 8;
    x &= -(uint32_t)(len > 0);

    // add ones and tens, in int8's, from 0x0[2]0[1] to 0x00[12]
    x = (x & 0x0f0f0f0fu) * ((1u << 8) * 10 + 1) >> 8;

    // add int16's, from 0x00[34]00[12] to 0x0000[1234]
    x = (x & 0x00ff00ffu) * ((1u << 16) * 100 + 1) >> 16;

    return x;
}

// Parse uint from string of up to 8 chars
inline uint32_t atou8(const char* s, uint32_t len) {
    assert(len <= 8);

    // int 64 of s. "12345678" --> 0x3837363534333231
    uint64_t x = cast<uint64_t>(s);

    // apply len. len of 2 --> 0x3231000000000000
    x <<= 64 - len * 8;
    x &= -(uint64_t)(len > 0);

    // add ones and tens, in int8's, from 0x0[2]0[1] to 0x00[12]
    x = (x & 0x0f0f0f0f0f0f0f0full) * ((1ull << 8) * 10 + 1) >> 8;

    // add int16's, from 0x00[34]00[12] to 0x0000[1234]
    x = (x & 0x00ff00ff00ff00ffull) * ((1ull << 16) * 100 + 1) >> 16;

    // add int32's, from 0x0000[1234]0000[5678] to 0x[12345678]
    x = (x & 0x0000ffff0000ffffull) * ((1ull << 32) * 10000 + 1) >> 32;

    return x;
}

// Parse uint64_t from string of up to 20 chars
// *** More than 20 char returns junk.
inline uint64_t atou(const char* s, uint32_t len) {
    assert(len <= 20);
    uint64_t x = 0;
    if (len > 8) {
        uint32_t lh = len % 8;
        x = atou8(s, lh);
        x *= 100000000;
        len -= lh;
        s += lh;
        if (len > 8) {
            x += atou8(s, 8);
            x *= 100000000;
            len -= 8;
            s += 8;
        }
    }
    return x + atou8(s, len);
}

// Parse _signed_ int from string of up to 20 chars. No spaces
inline int64_t atoi(const char* s, uint32_t len) {
    bool neg = !!len & (*s == '-');
    bool ls = !!len & (*s == '-' || *s == '+');
    s += ls;

    int64_t x = atou(s, len - ls);

    return neg ? -x : x;
}

// Parse hex int from string of up to 8 chars
inline uint32_t htou8(const char* s, uint32_t len) {
    assert(len <= 8);

    // int 64 of s. "12345678" --> 0x3837363534333231
    uint64_t x = cast<uint64_t>(s);

    // apply len. len of 2 --> 0x3231000000000000
    x <<= 64 - len * 8;

    // handle length of 0. remove all bits if zero
    x &= -(uint64_t)(len > 0);

    // change a-f to to number. 0x41 --> 0x0a
    x += ((x & 0x4040404040404040ull) >> 6) * 9;

    // add ones and 0x10's, in int8's, from 0x0[f]0[1] to 0x00[1f]
    x = (x & 0x0f0f0f0f0f0f0f0full) * ((1ull << 12) + 1) >> 8;

    // add int16's, from 0x00[ed]00[1f] to 0x0000[1fed]
    x = (x & 0x00ff00ff00ff00ffull) * ((1ull << 24) + 1) >> 16;

    // add int32's, from 0x0000[1fed]0000[cba9] to 0x[1fedcba9]
    x = (x & 0x0000ffff0000ffffull) * ((1ull << 48) + 1) >> 32;

    return x;
}

// Parse hex int from string of up to 16 chars
inline uint64_t htou(const char* s, uint32_t len) {
    assert(len <= 16);
    uint64_t x = 0;
    if (len > 8) {
        uint32_t lh = len - 8;
        x = htou8(s, lh);
        x <<= 32;
        len -= lh;
        s += lh;
    }
    return x + htou8(s, len);
}

//// int to string

// *** p suffix means zero-padded

// Convert uint, of less than 100, to %02u, as int 16
inline uint16_t utoa2p(uint64_t x) {
    static const CODE_SECTION uint8_t pairs[50] = { // 0..49, little endian
        0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90,
        0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x81, 0x91,
        0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 0x82, 0x92,
        0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, 0x83, 0x93,
        0x04, 0x14, 0x24, 0x34, 0x44, 0x54, 0x64, 0x74, 0x84, 0x94,
    };

    uint32_t b50 = -(uint32_t)(x >= 50); // x >= 50 ? ~0 : 0;
    uint32_t x2 = x - (50u & b50);       // x2 = x % 50;
    uint16_t t = pairs[x2] + (b50 & 5);  // t = pairs[x % 50] + 5 in low nibble if x > 50

    // move upper nibble to next byte and add '00'
    return ((t | (t << 4)) & 0x0f0f) | 0x3030;
}

// Convert uint, of less than 100, to %02u
inline void utoa2p(uint64_t x, char* s) {
    uint16_t t = utoa2p(x);
    memcpy(s, &t, sizeof(uint16_t));
}

// Convert uint to %0<N>u, N <= 8
template <int N>
inline uint64_t _utoap(uint64_t x, char* s) {
    static_assert(N <= 8);

    uint64_t tmp = utoa2p(x % 100);

    for (int i = 0; i < N - 2; i += 2) {
        x /= 100;
        tmp <<= 16;
        tmp |= utoa2p(x % 100);
    }

    tmp >>= (N & 1) * 8;
    memcpy(s, &tmp, 8);

    return x;
}

// Convert uint to %0<N>u, N <= 20
template <int N>
inline char* utoap(uint64_t x, char* s) {
    if constexpr (N <= 8) {
        _utoap<N>(x, s);
    }
    else if constexpr (N <= 16) {
        x = _utoap<N - 8>(x, s + 8);
        x /= (N & 1) ? 10 : 100;
        _utoap<8>(x, s);
    }
    else {
        x = _utoap<N - 16>(x, s + 16);
        x /= (N & 1) ? 10 : 100;
        x = _utoap<8>(x, s + 8);
        _utoap<8>(x / 100, s);
    }

    s[N] = '\0';
    return s;
}

// Convert signed int 32 to string of up to 8 bytes.
inline uint32_t itoa8(int32_t x, char* buf) {
    // Handle negatives
    bool neg = x < 0;
    *buf = '-'; // Always write
    buf += neg; // But advance only if negative
    x = __builtin_abs(x);

    uint64_t tmp = 0;
    int n = 0;

    // Convert pairs of digits
    while (x >= 100) {
        n += 2;
        tmp <<= 16;
        tmp |= utoa2p(x % 100);
        x /= 100;
    }

    // Last pair - no need to divide any more
    n += 2;
    tmp <<= 16;
    tmp |= utoa2p(x);

    // If last pair is 0<N> we want to remove this "0"
    n -= x < 10;
    tmp >>= x < 10 ? 8 : 0;

    // Copy to provided buffer
    memcpy(buf, &tmp, 8);
    buf[n] = '\0';

    return n + neg;
}

// Convert signed int 64 to string. String buffer is at least 22 bytes.
// Returns length
// *** this feels inefficient :( ***
inline uint32_t itoa(int64_t x, char* buf) {
    // Handle negatives
    bool neg = x < 0;
    *buf = '-'; // Always write
    buf += neg; // But advance only if negative
    x = __builtin_abs(x);

    char tmp[20];
    char* p = tmp + 20;

    while (x >= 100) {
        p -= 2;
        utoa2p(x % 100, p);
        x /= 100;
    }

    p -= 2;
    utoa2p(x, p);
    
    p += x < 10;

    uint32_t len = tmp + 20 - p;

    memcpy(buf, p, 20);
    buf[len] = '\0';

    return len + neg;
}

//// Double to string

// Copy the sign from src to dst that is unsigned.
// *** dst is up to 63 bit
inline int64_t _copySign(int64_t src, uint64_t dst) {
    // This is better than `src > 0 ? dst : -dst` that is using cmov
    uint64_t m = ~(src >> 63); // all 1s if >= 0 (opposite of abs)
    return (dst + m) ^ m; // flip if src negative
}

// Parse double from string
// *** More than 20 char integer part returns junk.
// *** Too much decimal char will get lost to precision
inline double atod(const char* s, uint32_t len) {
    // Get int part
    int ilen = pmemchr(s, len, '.');

    // If no decimal dot, return the int
    if (ilen == -1) {
        return atoi(s, len);
    }

    // Do the int part
    int64_t ipart = atoi(s, ilen);
    s += ilen + 1;
    len -= ilen + 1;

    // Int of decimal part
    int64_t dpart = atou(s, len);

    // To add the two parts we need matching signs
    dpart = _copySign(ipart, dpart);

    // Array of 20 * 8 = 160 bytes
    static const CODE_SECTION double scales[20] = {
        1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9, 1e-10,
        1e-11, 1e-12, 1e-13, 1e-14, 1e-15, 1e-16, 1e-17, 1e-18, 1e-19, 1e-20 };

    return ipart + dpart * scales[len];
}

} // namespace swar
