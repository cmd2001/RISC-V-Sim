#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include "decoder.hpp"
#include "executor.hpp"
#include "mem.hpp"

using namespace std;

int main() {
    RISC_V::Decoder d;
    while(1) {
        unsigned ins = 0, in[4];
        for(int i = 0; i < 4; i++) scanf("%X", in + i);
        for(int i = 3; ~i; i--) ins <<= 8, ins |= in[i];
        printf("%X\n", ins);
        d.decode(ins).print();
    }
    return 0;
}
