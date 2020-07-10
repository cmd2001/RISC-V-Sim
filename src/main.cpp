#include "../include/shared.hpp"
#include "decoder.hpp"
#include "executor.hpp"
#include "mem.hpp"
#include "other.hpp"


RISC_V::CPU c;

int main() {
    freopen("data.txt", "r", stdin);
    c.init(cin);
    c.work();
    return 0;
}
