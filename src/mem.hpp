#ifndef RISC_V_MEM_HPP
#define RISC_V_MEM_HPP

#include "../include/shared.hpp"
namespace RISC_V {

class Memory_Access {
    uint read(const Memory &m, const uint st, const uint len) {
        uint ret = 0;
        for(uint i = len - 1; ~i; i--) ret <<= 8, ret |= m[st + i]; // instead of <<= 1
        return ret; // don't forget return.
    }
    void write(Memory &m, const uint st, uint x, const uint len) {
        const uint mask = (1u << 8) - 1u;
        for(uint i = 0; i < len; i++) m[st + i] = x & mask, x >>= 8;
    }
    void writeSgn(uint &dst, const uint &startP) {
        const uint mask = - (1u << startP);
        dst |= mask;
    }
    uint extend(const uint ori, const uint oriLen) {
        uint sgn = (ori >> (oriLen - 1)) & 1, ret = ori;
        if(sgn) writeSgn(ret, oriLen);
        return ret;
    }
public:
    bool check(const EX2MEM &arg) {
        return arg.ins.tpe == LOAD || arg.ins.tpe == STORE;
    }
    MEM2WB work(const EX2MEM &arg, Memory &m) {
        Instruction ins = arg.ins;
        Registers_Diff r = arg.reg;
        if(ins.tpe == LOAD) r.rd_changed = 1;
        if(ins.tpe == LOAD || ins.tpe == STORE) {
            switch(ins.ins) {
                case LB:
                    r.rd_val = extend(read(m, ins.imm, 1), 8);
                    break;
                case LBU:
                    r.rd_val = read(m, ins.imm, 1);
                    break;
                case LH:
                    r.rd_val = extend(read(m, ins.imm, 2), 16);
                    break;
                case LHU:
                    r.rd_val = read(m, ins.imm, 2);
                    break;
                case LW:
                    r.rd_val = read(m, ins.imm, 4);
                    break;
                case SB:
                    write(m, ins.imm, r.rs2_val, 1);
                    break;
                case SH:
                    write(m, ins.imm, r.rs2_val, 2);
                    break;
                case SW:
                    write(m, ins.imm, r.rs2_val, 4);
                    break;
                default:
                    debug << "BAD INSTRUCTION IN MEMORY_ACCESS::WORK()" << endl;
                    assert(0);
            }
        }
        return (MEM2WB){ins, r};
    }
};

}
#endif //RISC_V_MEM_HPP
