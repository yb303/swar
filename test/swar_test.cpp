#include "../swar.h"
#include <stdlib.h>
#include <gtest/gtest.h>
#include <limits>


TEST(r8, memchr) {
    EXPECT_EQ(swar::memchr8("12345678=90", '='), -1);
    EXPECT_EQ(swar::memchr8("1234567=890", '='), 7);
    EXPECT_EQ(swar::memchr8("123456=7890", '='), 6);
    EXPECT_EQ(swar::memchr8("12345=67890", '='), 5);
    EXPECT_EQ(swar::memchr8("1234=567890", '='), 4);
    EXPECT_EQ(swar::memchr8("123=4567890", '='), 3);
    EXPECT_EQ(swar::memchr8("12=34567890", '='), 2);
    EXPECT_EQ(swar::memchr8("1=234567890", '='), 1);
    EXPECT_EQ(swar::memchr8("=1234567890", '='), 0);

    EXPECT_EQ(swar::memchr8("1234=", '='), 4);
    EXPECT_EQ(swar::memchr8("123=4", '='), 3);
    EXPECT_EQ(swar::memchr8("12=34", '='), 2);
    EXPECT_EQ(swar::memchr8("1=234", '='), 1);
    EXPECT_EQ(swar::memchr8("=1234", '='), 0);

    EXPECT_EQ(swar::memchr8("===", '='), 0);
    EXPECT_EQ(swar::memchr8("==", '='), 0);
    EXPECT_EQ(swar::memchr8("=", '='), 0);

    //                    123456789 123456789 123456789 
    EXPECT_EQ(swar::memchr("1234567890abcdefghij=", 20, '='), -1);
    EXPECT_EQ(swar::memchr("12345678=90abcdefghi", 20, '='), 8);
    EXPECT_EQ(swar::memchr("1234=567890abcdefghi", 20, '='), 4);

    char nc[24] = "1234567890abcdefghij=12";
    EXPECT_EQ(swar::memchr_nc(nc, 20, '='), -1);
    EXPECT_EQ(swar::memchr_nc(nc, 21, '='), 20);
    EXPECT_EQ(swar::memchr_nc(nc, 23, '='), 20);

}

TEST(r8, cast8) {
    EXPECT_EQ(swar::cast8("1234567890", 0),                      0);
    EXPECT_EQ(swar::cast8("1234567890", 1),                0x31ull);
    EXPECT_EQ(swar::cast8("1234567890", 2),              0x3231ull);
    EXPECT_EQ(swar::cast8("1234567890", 3),            0x333231ull);
    EXPECT_EQ(swar::cast8("1234567890", 4),          0x34333231ull);
    EXPECT_EQ(swar::cast8("1234567890", 5),        0x3534333231ull);
    EXPECT_EQ(swar::cast8("1234567890", 6),      0x363534333231ull);
    EXPECT_EQ(swar::cast8("1234567890", 7),    0x37363534333231ull);
    EXPECT_EQ(swar::cast8("1234567890", 8),  0x3837363534333231ull);
}

TEST(r8, atoi) {
    // short
    EXPECT_EQ(swar::atou4("1234567890", 0),  0);
    EXPECT_EQ(swar::atou4("1234567890", 1),  1);
    EXPECT_EQ(swar::atou4("1234567890", 2),  12);
    EXPECT_EQ(swar::atou4("1234567890", 3),  123);
    EXPECT_EQ(swar::atou4("1234567890", 4),  1234);

    EXPECT_EQ(swar::atou8("1234567890", 0),  0);
    EXPECT_EQ(swar::atou8("1234567890", 1),  1);
    EXPECT_EQ(swar::atou8("1234567890", 2),  12);
    EXPECT_EQ(swar::atou8("1234567890", 3),  123);
    EXPECT_EQ(swar::atou8("1234567890", 4),  1234);
    EXPECT_EQ(swar::atou8("1234567890", 5),  12345);
    EXPECT_EQ(swar::atou8("1234567890", 6),  123456);
    EXPECT_EQ(swar::atou8("1234567890", 7),  1234567);
    EXPECT_EQ(swar::atou8("1234567890", 8),  12345678);

    // long
    EXPECT_EQ(swar::atou("12345678901234567890",  0), 0ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  1), 1ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  2), 12ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  3), 123ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  4), 1234ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  5), 12345ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  6), 123456ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  7), 1234567ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  8), 12345678ull);
    EXPECT_EQ(swar::atou("12345678901234567890",  9), 123456789ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 10), 1234567890ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 11), 12345678901ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 12), 123456789012ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 13), 1234567890123ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 14), 12345678901234ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 15), 123456789012345ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 16), 1234567890123456ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 17), 12345678901234567ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 18), 123456789012345678ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 19), 1234567890123456789ull);
    EXPECT_EQ(swar::atou("12345678901234567890", 20), 12345678901234567890ull);

    // long signed w/o sign
    EXPECT_EQ(swar::atoi("12345678901234567890",  0), 0);
    EXPECT_EQ(swar::atoi("12345678901234567890",  1), 1ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  2), 12ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  3), 123ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  4), 1234ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  5), 12345ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  6), 123456ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  7), 1234567ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  8), 12345678ll);
    EXPECT_EQ(swar::atoi("12345678901234567890",  9), 123456789ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 10), 1234567890ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 11), 12345678901ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 12), 123456789012ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 13), 1234567890123ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 14), 12345678901234ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 15), 123456789012345ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 16), 1234567890123456ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 17), 12345678901234567ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 18), 123456789012345678ll);
    EXPECT_EQ(swar::atoi("12345678901234567890", 19), 1234567890123456789ll);

    // long signed +
    EXPECT_EQ(swar::atoi("+12345678901234567890",  2), 1ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890",  3), 12ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890",  4), 123ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890",  5), 1234ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890",  6), 12345ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890",  7), 123456ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890",  8), 1234567ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890",  9), 12345678ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 10), 123456789ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 11), 1234567890ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 12), 12345678901ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 13), 123456789012ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 14), 1234567890123ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 15), 12345678901234ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 16), 123456789012345ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 17), 1234567890123456ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 18), 12345678901234567ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 19), 123456789012345678ll);
    EXPECT_EQ(swar::atoi("+12345678901234567890", 20), 1234567890123456789ll);

    // long signed -
    EXPECT_EQ(swar::atoi("-12345678901234567890",  2), -1ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890",  3), -12ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890",  4), -123ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890",  5), -1234ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890",  6), -12345ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890",  7), -123456ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890",  8), -1234567ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890",  9), -12345678ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 10), -123456789ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 11), -1234567890ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 12), -12345678901ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 13), -123456789012ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 14), -1234567890123ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 15), -12345678901234ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 16), -123456789012345ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 17), -1234567890123456ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 18), -12345678901234567ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 19), -123456789012345678ll);
    EXPECT_EQ(swar::atoi("-12345678901234567890", 20), -1234567890123456789ll);
}

TEST(r8, htou) {
    EXPECT_EQ(swar::htou8("123456789abcdef0", 0), 0);
    EXPECT_EQ(swar::htou8("123456789abcdef0", 1), 0x1);
    EXPECT_EQ(swar::htou8("123456789abcdef0", 2), 0x12);
    EXPECT_EQ(swar::htou8("123456789abcdef0", 3), 0x123);
    EXPECT_EQ(swar::htou8("123456789abcdef0", 4), 0x1234);
    EXPECT_EQ(swar::htou8("123456789abcdef0", 5), 0x12345);
    EXPECT_EQ(swar::htou8("123456789abcdef0", 6), 0x123456);
    EXPECT_EQ(swar::htou8("123456789aBCDEf0", 7), 0x1234567);
    EXPECT_EQ(swar::htou8("123456789aBCDEf0", 8), 0x12345678);

    EXPECT_EQ(swar::htou("123456789abcdef0",  0), 0ull);
    EXPECT_EQ(swar::htou("123456789abcdef0",  1), 0x1ull);
    EXPECT_EQ(swar::htou("123456789abcdef0",  2), 0x12ull);
    EXPECT_EQ(swar::htou("123456789abcdef0",  3), 0x123ull);
    EXPECT_EQ(swar::htou("123456789abcdef0",  4), 0x1234ull);
    EXPECT_EQ(swar::htou("123456789abcdef0",  5), 0x12345ull);
    EXPECT_EQ(swar::htou("123456789abcdef0",  6), 0x123456ull);
    EXPECT_EQ(swar::htou("123456789aBCDEf0",  7), 0x1234567ull);
    EXPECT_EQ(swar::htou("123456789aBCDEf0",  8), 0x12345678ull);
    EXPECT_EQ(swar::htou("123456789abCDEF0",  9), 0x123456789ull);
    EXPECT_EQ(swar::htou("123456789abCDEF0", 10), 0x123456789aull);
    EXPECT_EQ(swar::htou("123456789abCDEF0", 11), 0x123456789abull);
    EXPECT_EQ(swar::htou("123456789abCDEF0", 12), 0x123456789abcull);
    EXPECT_EQ(swar::htou("123456789abCDEF0", 13), 0x123456789abcdull);
    EXPECT_EQ(swar::htou("123456789ABcdef0", 14), 0x123456789abcdeull);
    EXPECT_EQ(swar::htou("123456789ABcdef0", 15), 0x123456789abcdefull);
    EXPECT_EQ(swar::htou("123456789ABcdef0", 16), 0x123456789abcdef0ull);

    EXPECT_EQ(swar::htou8("abcdef..", 1),  0xa);
    EXPECT_EQ(swar::htou8("abcdef..", 2),  0xab);
    EXPECT_EQ(swar::htou8("abcdef..", 3),  0xabc);
    EXPECT_EQ(swar::htou8("abcdef..", 4),  0xabcd);
    EXPECT_EQ(swar::htou8("abcdef..", 5),  0xabcde);
    EXPECT_EQ(swar::htou8("abcdef..", 6),  0xabcdef);

    EXPECT_EQ(swar::htou8("ABCDEF..", 1),  0xa);
    EXPECT_EQ(swar::htou8("ABCDEF..", 2),  0xab);
    EXPECT_EQ(swar::htou8("ABCDEF..", 3),  0xabc);
    EXPECT_EQ(swar::htou8("ABCDEF..", 4),  0xabcd);
    EXPECT_EQ(swar::htou8("ABCDEF..", 5),  0xabcde);
    EXPECT_EQ(swar::htou8("ABCDEF..", 6),  0xabcdef);

    EXPECT_EQ(swar::htou8("abef0189", 5),  0xabef0);
    EXPECT_EQ(swar::htou8("abef0189", 6),  0xabef01);
    EXPECT_EQ(swar::htou8("abef0189", 7),  0xabef018);
    EXPECT_EQ(swar::htou8("abef0189", 8),  0xabef0189);

    EXPECT_EQ(swar::htou8("1234abef", 5),  0x1234a);
    EXPECT_EQ(swar::htou8("1234abef", 6),  0x1234ab);
    EXPECT_EQ(swar::htou8("1234abef", 7),  0x1234abe);
    EXPECT_EQ(swar::htou8("1234abef", 8),  0x1234abef);
}

TEST(r8, itoa) {
    union {
        char test_buf[100];
        uint16_t test16;
    };
    char itoa_ret[100];

    for (int i = -100000; i < 100000; i++) {
        sprintf(test_buf, "%d", i);
        swar::itoa(i, itoa_ret);
        EXPECT_STREQ(itoa_ret, test_buf);
    }

    for (int64_t i = std::numeric_limits<int64_t>::min();
        i < std::numeric_limits<int64_t>::min(); i += 13371) {
        sprintf(test_buf, "%ld", i);
        swar::itoa(i, itoa_ret);
        EXPECT_STREQ(itoa_ret, test_buf);
    }

    EXPECT_STREQ(swar::utoap< 1>(0, itoa_ret), "0");
    EXPECT_STREQ(swar::utoap< 2>(0, itoa_ret), "00");
    EXPECT_STREQ(swar::utoap< 3>(0, itoa_ret), "000");
    EXPECT_STREQ(swar::utoap< 4>(0, itoa_ret), "0000");
    EXPECT_STREQ(swar::utoap< 5>(0, itoa_ret), "00000");
    EXPECT_STREQ(swar::utoap< 6>(0, itoa_ret), "000000");
    EXPECT_STREQ(swar::utoap< 7>(0, itoa_ret), "0000000");
    EXPECT_STREQ(swar::utoap< 8>(0, itoa_ret), "00000000");
    EXPECT_STREQ(swar::utoap< 9>(0, itoa_ret), "000000000");
    EXPECT_STREQ(swar::utoap<10>(0, itoa_ret), "0000000000");
    EXPECT_STREQ(swar::utoap<11>(0, itoa_ret), "00000000000");
    EXPECT_STREQ(swar::utoap<12>(0, itoa_ret), "000000000000");
    EXPECT_STREQ(swar::utoap<13>(0, itoa_ret), "0000000000000");
    EXPECT_STREQ(swar::utoap<14>(0, itoa_ret), "00000000000000");
    EXPECT_STREQ(swar::utoap<15>(0, itoa_ret), "000000000000000");
    EXPECT_STREQ(swar::utoap<16>(0, itoa_ret), "0000000000000000");
    EXPECT_STREQ(swar::utoap<17>(0, itoa_ret), "00000000000000000");
    EXPECT_STREQ(swar::utoap<18>(0, itoa_ret), "000000000000000000");
    EXPECT_STREQ(swar::utoap<19>(0, itoa_ret), "0000000000000000000");

    EXPECT_STREQ(swar::utoap< 0>(7,               itoa_ret), "");
    EXPECT_STREQ(swar::utoap< 1>(7,               itoa_ret), "7");
    EXPECT_STREQ(swar::utoap< 2>(7,               itoa_ret), "07");
    EXPECT_STREQ(swar::utoap< 3>(7,               itoa_ret), "007");
    EXPECT_STREQ(swar::utoap< 4>(7,               itoa_ret), "0007");
    EXPECT_STREQ(swar::utoap< 5>(7,               itoa_ret), "00007");
    EXPECT_STREQ(swar::utoap< 6>(12345,           itoa_ret), "012345");
    EXPECT_STREQ(swar::utoap< 7>(12345,           itoa_ret), "0012345");
    EXPECT_STREQ(swar::utoap< 8>(12345,           itoa_ret), "00012345");
    EXPECT_STREQ(swar::utoap< 9>(12345,           itoa_ret), "000012345");
    EXPECT_STREQ(swar::utoap<10>(12345,           itoa_ret), "0000012345");
    EXPECT_STREQ(swar::utoap<11>(12345,           itoa_ret), "00000012345");
    EXPECT_STREQ(swar::utoap<12>(12345678901,     itoa_ret), "012345678901");
    EXPECT_STREQ(swar::utoap<13>(12345678901,     itoa_ret), "0012345678901");
    EXPECT_STREQ(swar::utoap<14>(12345678901,     itoa_ret), "00012345678901");
    EXPECT_STREQ(swar::utoap<15>(12345678901,     itoa_ret), "000012345678901");
    EXPECT_STREQ(swar::utoap<16>(12345678901,     itoa_ret), "0000012345678901");
    EXPECT_STREQ(swar::utoap<17>(123456789012345, itoa_ret), "00123456789012345");
    EXPECT_STREQ(swar::utoap<18>(123456789012345, itoa_ret), "000123456789012345");
    EXPECT_STREQ(swar::utoap<19>(123456789012345, itoa_ret), "0000123456789012345");
    EXPECT_STREQ(swar::utoap<20>(123456789012345, itoa_ret), "00000123456789012345");

    swar::itoa8(0,        itoa_ret); EXPECT_STREQ(itoa_ret, "0");
    swar::itoa8(1,        itoa_ret); EXPECT_STREQ(itoa_ret, "1");
    swar::itoa8(12,       itoa_ret); EXPECT_STREQ(itoa_ret, "12");
    swar::itoa8(123,      itoa_ret); EXPECT_STREQ(itoa_ret, "123");
    swar::itoa8(1234,     itoa_ret); EXPECT_STREQ(itoa_ret, "1234");
    swar::itoa8(12345,    itoa_ret); EXPECT_STREQ(itoa_ret, "12345");
    swar::itoa8(123456,   itoa_ret); EXPECT_STREQ(itoa_ret, "123456");
    swar::itoa8(1234567,  itoa_ret); EXPECT_STREQ(itoa_ret, "1234567");
    swar::itoa8(12345678, itoa_ret); EXPECT_STREQ(itoa_ret, "12345678");
}


