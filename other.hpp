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
        dst.merge(arg.reg);
    }
};

class BranchPredictor {
private:
    uint val; // in range 0-3, 0,1 means jump, 2,3 means not jump
    uint sum, fail_times;
public:
    BranchPredictor(): val(1), sum(0), fail_times(0) {};
    bool getPre() { /* return val <= 1;*/ return 0; } // used to check.
    void fail() {
        static uint nxt[] = {1, 2, 1, 2};
        ++fail_times, val = nxt[val];
    }
    void suc() {
        static uint nxt[] = {0, 0, 2, 2};
        val = nxt[val];
    }
    void print() {
        debug << "Prediction Success Rate = " << (long double)(sum - fail_times) / sum << endl;
    }
};

}
#endif //RISC_V_OTHER_HPP
