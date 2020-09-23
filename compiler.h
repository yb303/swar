#pragma once

#if defined(__linux__) && defined(__GNUC__)
#define CODE_SECTION __attribute__ ((section (".text#")))
#else
#define CODE_SECTION
#endif

#if defined(__GNUC__)
#define likely(X) __builtin_expect(!!(X), 1)
#define unlikely(X) __builtin_expect(!!(X), 0)
#else
#define likely(X) X
#define unlikely(X) X
#endif

