#include "../swar.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <random>

inline int64_t rdtsc() {
    union {
        struct { uint32_t lo, hi; };
        int64_t ts;
    } u;

    asm volatile("rdtsc" : "=a"(u.lo), "=d"(u.hi) : : "memory");
    return u.ts;
}

inline naive_atoull(const char* p, int n) {
    uint64_t ret = 0;
    for (int i = 0; i < n; i++)
        ret = ret * 10 + p[i] - '0';
    return ret;
}

void acc(uint64_t& dst, uint64_t src)
{
    if (dst == 0)
        dst = src;
    else if (src < dst)
        dst = src;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // The default input filename is executable full path but with .txt
    std::string filename = std::string(argv[0]) + ".txt";

    int test_size = 10000;
    int test_repetitions = 10;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            test_size = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-r") == 0) {
            test_repetitions = atoi(argv[++i]);
        }
    }

    // Generate a long string of random numbers for atoi
    std::vector<std::vector<char>> v(21);
    std::mt19937_64 mt(rdtsc());
    uint64_t mask = 1;
    for (int len = 1; len < 21; len++) {
        v[len].resize(test_size * (len + 1));
        mask *= 10;
        int total_len = 0;
        char* buf = v[len].data();
        for (int i = 1; i < test_size; i++) {
            sprintf(buf + total_len, "%0lu", mt() % mask);
            total_len += len + 1;
        }
    }

    uint64_t junk = 0;
    std::vector<uint64_t> dt_no_op(21);
    std::vector<uint64_t> dt_stock(21);
    std::vector<uint64_t> dt_naive(21);
    std::vector<uint64_t> dt_swar_(21);
    std::vector<uint64_t> dt_swar8(21);
    std::vector<uint64_t> dt_swar4(21);

    for (int r = 0; r < test_repetitions; r++) {
        for (int len = 1; len < 21; len++) {
            char* buf = v[len].data();

            // Test no-op
            uint64_t t0 = rdtsc();
            int total_len = 0;
            for (int i = 0; i < test_size; i++ ) {
                junk += buf[total_len];
                total_len += len + 1;
            }

            // Test stock atoull
            uint64_t t1 = rdtsc();
            total_len = 0;
            for (int i = 0; i < test_size; i++ ) {
                junk += atoll(buf + total_len);
                total_len += len + 1;
            }

            // Test naive atoull
            uint64_t t2 = rdtsc();
            total_len = 0;
            for (int i = 0; i < test_size; i++ ) {
                junk += naive_atoull(buf + total_len, len);
                total_len += len + 1;
            }

            // Test swar atou
            uint64_t t3 = rdtsc();
            total_len = 0;
            for (int i = 0; i < test_size; i++ ) {
                junk += swar::atou(buf + total_len, len);
                total_len += len + 1;
            }

            // Test swar atou8
            int len8 = len <= 8 ? len : 8;
            uint64_t t4 = rdtsc();
            total_len = 0;
            for (int i = 0; i < test_size; i++ ) {
                junk += swar::atou8(buf + total_len, len8);
                total_len += len + 1;
            }

            // Test swar atou4
            int len4 = len <= 4 ? len : 4;
            uint64_t t5 = rdtsc();
            total_len = 0;
            for (int i = 0; i < test_size; i++ ) {
                junk += swar::atou4(buf + total_len, len4);
                total_len += len + 1;
            }

            uint64_t t6 = rdtsc();
            acc(dt_no_op[len], t1 - t0);
            acc(dt_stock[len], t2 - t1);
            acc(dt_naive[len], t3 - t2);
            acc(dt_swar_[len], t4 - t3);
            acc(dt_swar8[len], t5 - t4);
            acc(dt_swar4[len], t6 - t5);
        }
    }

    printf("%d%c", uint32_t(junk) % 10, 8);
    printf("len %7s %7s %7s %7s %7s\n",
           "stock", "naive", "swar", "swar8", "swar4");
    double f = 1.0 / test_size;
    for (int len = 1; len < 21; len++) {
        double tf_stock = (dt_stock[len] - dt_no_op[len]) * f;
        double tf_naive = (dt_naive[len] - dt_no_op[len]) * f;
        double tf_swar_ = (dt_swar_[len] - dt_no_op[len]) * f;
        double tf_swar8 = (dt_swar8[len] - dt_no_op[len]) * f;
        double tf_swar4 = (dt_swar4[len] - dt_no_op[len]) * f;
        printf("%3d %7.1f %7.1f %7.1f %7.1f %7.1f\n",
               len, tf_stock, tf_naive, tf_swar_, tf_swar8, tf_swar4);
    }

    return 0;
}


