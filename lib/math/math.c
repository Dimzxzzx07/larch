#include <stdint.h>

int abs(int x) {
    return x < 0 ? -x : x;
}

long labs(long x) {
    return x < 0 ? -x : x;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

uint32_t pow_u32(uint32_t base, uint32_t exp) {
    uint32_t result = 1;
    while (exp) {
        if (exp & 1) result *= base;
        base *= base;
        exp >>= 1;
    }
    return result;
}

int div_round_up(int a, int b) {
    return (a + b - 1) / b;
}

int is_power_of_two(unsigned int x) {
    return x && !(x & (x - 1));
}