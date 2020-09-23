#include "compiler.h"

#include <assert.h>
#include <string.h> // for memcpy, memset
#include <stdint.h>

namespace swar {

//
// Function naming convention <prefix>: <function> <length> <suffix>
//  Length
//                  8 means input is 8 bytes
//                  4 means input is 4 bytes
//                  None means input is any length
//  Prefix
//          p       Printable. Ascii 0 to 127.
//                  pmemchr vs memchr is slighly optimized for printable input
//          _       Mostly internal use
//  Suffix
//          k       Haystack is known to contain needle
//          _nc     Non-const, modifiable, input
//                  These functions modify and restore the input. Not thread safe
//
// Performance
//  Functions with 8 suffix are branchless. Function with longer input must
//  have a branch per word. This can be improved with SSE.
//  - SSE may switch some processors to different P state, if the BIOS allows,
//    and the switching itself can take a few hundred cycles
//  - Branchless code is not always faster than branched code.
//    Lab tests are typically less impacted by branch miss-predictions, then real
//    world applications.
//    Loops may be fully predicted, if BP caches are all working for the test.
//    However, in a real world app, using branchless low level code means that BP
//    caches have more room for the application logic so the app as a whole may
//    become faster.
//    The only way to know for sure, is to test within the app.
//

//
// Utils
//

// Cast char* to T using memcpy. memcpy is optimized away on x86
template <typename T> inline T cast(const char* src);

// Get the uint _cast_ of a string of up to 8 chars
inline uint64_t cast8(const char* s, uint32_t len);

// Fill T with c's
template <typename T> inline T extend(char c);

// swap bytes
inline uint64_t bswap(uint64_t x);
inline uint32_t bswap(uint32_t x);
inline uint16_t bswap(uint16_t x);

//
// Find byte in word
//

// Check if word has zero byte
inline bool haszero(uint64_t x);

// Check if word has some byte
inline bool hasbyte(uint64_t x, uint8_t c);

// Find char in string. Support all options.
template<bool Printable, bool Exists, bool Reverse=false>
inline uint32_t _memchr8(const char* s, uint8_t c);

// Find char in string and trim it
template<bool Printable, bool Exists>
inline uint32_t _trim8(const char* s, uint8_t c);

// Find char in printable (chars < 128) string of 8 chars
inline uint32_t pmemchr8(const char* s, uint8_t c);

// Find char in printable (chars < 128) string of 8 chars
// * The string is known to contain the char
inline uint32_t pmemchr8k(const char* s, uint8_t c);

// Find char in binary string of 8 chars
inline uint32_t memchr8(const char* s, uint8_t c);

// Find char in binary string of 8 chars
// * The string is known to contain the char
inline uint32_t memchr8k(const char* s, uint8_t c);

//
// Strlen variants
//

// Find zero byte in binary string up to 8 chars
inline uint32_t strlen8(const char* s);

// Find zero byte in printable string up to 8 chars
inline uint32_t pstrlen8(const char* s);

// Find zero byte in binary string
inline uint32_t strlen(const char* s);

// Find zero byte in printable string
inline uint32_t pstrlen(const char* s);

//
// Find byte in word - reverse
//

// Find char in printable (chars < 128) string of 8 chars
inline uint32_t pmemrchr8(const char* s, uint8_t c);

// Find char in printable (chars < 128) string of 8 chars
// * The string is known to contain the char
inline uint32_t pmemrchr8k(const char* s, uint8_t c);

// Find char in binary string of 8 chars
inline uint32_t memrchr8(const char* s, uint8_t c);

// Find char in binary string of 8 chars
// * The string is known to contain the char
inline uint32_t memrchr8k(const char* s, uint8_t c);

//
// Find byte in const string. Like memchr
//

// Find char in const binary string
template<bool Printable, bool Known>
inline uint32_t _memchr(const char* s, uint32_t len, uint8_t c);

// Find char in binary string
inline uint32_t memchr(const char* s, uint32_t len, uint8_t c);

// Find char in binary string. Char c is known to be in s + len
inline uint32_t memchrk(const char* s, uint32_t len, uint8_t c);

// Find char in printable string
inline uint32_t pmemchr(const char* s, uint32_t len, uint8_t c);

// Find char in printable string. Char c is known to be in s + len
inline uint32_t pmemchrk(const char* s, uint32_t len, uint8_t c);

//
// Find byte, from end, in const string. Like memrchr
//

// Find char, in reverse, in const binary string
template<bool Printable, bool Known>
inline uint32_t _memrchr(const char* s, uint32_t len, uint8_t c);

// Find char in binary string
inline uint32_t memrchr(const char* s, uint32_t len, uint8_t c);

// Find char in binary string. Char c is known to be in s + len
inline uint32_t memrchrk(const char* s, uint32_t len, uint8_t c);

// Find char in printable string
inline uint32_t pmemrchr(const char* s, uint32_t len, uint8_t c);

// Find char in printable string. Char c is known to be in s + len
inline uint32_t pmemrchrk(const char* s, uint32_t len, uint8_t c);

//
// Find byte in NON-CONST string
//

template<bool Printable>
inline uint32_t _memchr_nc(char* s, uint32_t len, uint8_t c);

// Find char in binary NON-CONST string
inline uint32_t memchr_nc(char* s, uint32_t len, uint8_t c);

// Find char in printable NON-CONST string
inline uint32_t pmemchr_nc(char* s, uint32_t len, uint8_t c);

//// string to int

// Parse uint from string of up to 4 chars
inline uint16_t atou4(const char* s, uint32_t len);

// Parse uint from string of up to 8 chars
inline uint32_t atou8(const char* s, uint32_t len);

// Parse uint64_t from string of up to 20 chars
// *** More than 20 char returns junk.
inline uint64_t atou(const char* s, uint32_t len);

// Parse _signed_ int from string of up to 20 chars. No spaces
inline int64_t atoi(const char* s, uint32_t len);

// Parse hex int from string of up to 8 chars
inline uint32_t htou8(const char* s, uint32_t len);

// Parse hex int from string of up to 16 chars
inline uint64_t htou(const char* s, uint32_t len);

//// int to string

// *** p suffix means zero-padded

// Convert uint, of less than 100, to %02u, as int 16
inline uint16_t utoa2p(uint64_t x);

// Convert uint, of less than 100, to %02u
inline void utoa2p(uint64_t x, char* s);

// Convert uint to %0<N>u, N <= 8
template <int N>
inline uint64_t _utoap(uint64_t x, char* s);

// Convert uint to %0<N>u, N <= 20
template <int N>
inline char* utoap(uint64_t x, char* s);

// Convert signed int 32 to string of up to 8 bytes.
inline uint32_t itoa8(int32_t x, char* buf);

// Convert signed int 64 to string. String buffer is at least 22 bytes.
// Returns length
// *** this feels inefficient :( ***
inline uint32_t itoa(int64_t x, char* buf);

//// Double to string

// Copy the sign from src to dst that is unsigned.
// *** dst is up to 63 bit
inline int64_t _copySign(int64_t src, uint64_t dst);

// Parse double from string
// *** More than 20 char integer part returns junk.
// *** Too much decimal char will get lost to precision
inline double atod(const char* s, uint32_t len);

} // namespace swar
