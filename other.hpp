#ifndef RISC_V_OTHER_HPP
#define RISC_V_OTHER_HPP

#include "include/shared.hpp"
namespace RISC_V {

class Instruction_Fetcher {
public:
    uint fetch(Registers &r, const Memory &mem) {
        assert(r.pc % 4 == 0);
        uint ret = 0;
        for (uint i = 3; ~i; i--) ret <<= 1, ret |= mem[r.pc + i];
        r.pc += 4;
        return ret;
    }
};
class WriteBacker {
public:
    void writeBack(Registers &dst, const Registers &src) {
        dst = src;
    }
};

}
#endif //RISC_V_OTHER_HPP
