#ifndef RISC_V_OTHER_HPP
#define RISC_V_OTHER_HPP

#include "include/shared.hpp"
namespace RISC_V {

class Instruction_Fetcher {
public:
    IF2ID fetch(uint &pc, const Memory &mem) {
        assert(pc % 4 == 0);
        uint ret = 0;
        for (uint i = 3; ~i; i--) ret <<= 8, ret |= mem[pc + i];
        pc += 4;
        return (IF2ID){ret};
    }
};
class WriteBacker {
public:
    void writeBack(Registers &dst, const MEM2WB &arg) {
        dst = arg.reg;
    }
};

}
#endif //RISC_V_OTHER_HPP
