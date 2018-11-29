#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t xorshift32(uint32_t state[static 1]) {
    uint32_t x = state[0];
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state[0] = x;
    return x;
}

int main() {
    uint32_t i, *r = (uint32_t*) calloc(25, sizeof(uint32_t)), secret_key = 1;
    r[0] = secret_key;
    for(i = 1; i < 25; i ++) {
        *(r + i) = xorshift32(&secret_key);
    }
    for(i = 0; i < 25; i ++) {
        printf("%d ", *(r + i));
    }
    return 0;
}