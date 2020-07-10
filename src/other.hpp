#ifndef RISC_V_OTHER_HPP
#define RISC_V_OTHER_HPP

#include "../include/shared.hpp"
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
    BranchPredictor(): val(2), sum(0), fail_times(0) {};
    bool predict() { ++sum; return val <= 1; }
    bool getPre() { return val <= 1; } // used to check.
    void fail() {
        static uint nxt[] = {1, 2, 1, 2};
        ++fail_times, val = nxt[val];
    }
    void suc() {
        static uint nxt[] = {0, 0, 2, 2};
        val = nxt[val];
    }
    uint jmpc(const ID2EX &arg) {
        return arg.reg.pc_val - 4 + arg.ins.imm;
    }
    void print() {
        debug << "Prediction Success Rate = " << (long double)(sum - fail_times) / sum << endl;
    }
};

class CPU {
private:
    Instruction_Fetcher IF;
    Decoder ID;
    Executor EX;
    Memory_Access MEM;
    WriteBacker WB;

    BranchPredictor BP;

    Memory mem;
    Registers reg;

    struct Clock {
        IF2ID t1;
        ID2EX t2;
        EX2MEM t3;
        MEM2WB t4;
        Registers_Diff reg_New;
        bool vaild[4];
    };

    bool flag;

    char count[32];

    Clock stepClock(Clock &c, const uint clo) { // move 1 clock forward.
        static uint lastMem = uint(-1);
        Clock ret;
        for (uint i = 1; i < 4; i++) ret.vaild[i] = c.vaild[i - 1];
        ret.vaild[0] = 1;
        if (c.vaild[3]) {
            WB.writeBack(reg, c.t4);

            // if(c.t2.ins.rd != uint(-1)) --count[c.t2.ins.rd]; // usage counter.

            if (c.t4.ins.oriIns == endIns) {
                flag = 1;
                return ret;
            }
        }
        if (c.vaild[2]) {
            if (MEM.check(c.t3)) {
                if (lastMem == uint(-1)) lastMem = clo;
                if (clo != lastMem + 3) { // not enough delay
                    ret.t3 = c.t3, ret.t2 = c.t2, ret.t1 = c.t1;
                    ret.vaild[3] = 0;
                    return ret;
                } else {
                    lastMem = uint(-1);
                    if (c.t3.ins.tpe == LOAD) { // stall 1 clock by overriding data from EX, ID, IF.
                        ret.t4 = MEM.work(c.t3, mem);
                        ret.t2 = c.t2, ret.t1 = c.t1, ret.vaild[2] = 0;
                        ret.reg_New = ret.t4.reg; // forwarding: short path
                        return ret;
                    } else ret.t4 = MEM.work(c.t3, mem); // STORE.
                }
            } else ret.t4 = MEM.work(c.t3, mem);
        }
        if (c.vaild[1]) {
            c.t2.reg.merge(c.reg_New); // forwarding: update
            ret.t3 = EX.execute(c.t2);

            // if(c.t2.ins.rd != uint(-1)) ++count[c.t2.ins.rd]; // usage counter.

            if (ret.t3.ins.tpe == JMP && ret.t3.ins.oriIns != uint(-1)) {
                reg.pc = ret.t3.reg.pc_val, ret.t3.reg.pc_changed = 0;
                ret.vaild[0] = ret.vaild[1] = 0;
                return ret;
            } else if (ret.t3.ins.tpe == JMPC) { // clear all level
                if (ret.t3.reg.pc_changed != BP.getPre()) {
                    ret.t3.reg.pc_changed = 0; // avoid changing pc again.
                    if (BP.getPre()) reg.pc = ret.t3.reg.pc_val = ret.t3.reg.rd_val; // fail back
                    else reg.pc = ret.t3.reg.pc_val; // do jump
                    BP.fail();
                    ret.vaild[0] = ret.vaild[1] = 0; // clear pipeline after jump failure.
                    return ret;
                } else ret.t3.reg.pc_changed = 0, BP.suc(); // avoid changing pc again.
            }
            ret.reg_New = ret.t3.reg; // after with ret.t3.reg.pc_changed = 0.
        }
        if (c.vaild[0]) {
            ret.t2 = ID.decode(c.t1, reg);
            ret.t2.reg.merge(c.reg_New); // forwarding: update
            /* if (ret.t2.ins.tpe == JMP && (ret.t2.ins.ins == JAL || !count[ret.t2.ins.rs1]) ) {
                reg.pc = ID.jmp(ret.t2), ret.t2.reg.pc_changed = 0; // override PC, which can be implemented by circuit.
                ret.t2.ins.oriIns = uint(-1); // mark it as solved.
            } */
            if (ret.t2.ins.tpe == JMPC) {
                ret.t2.reg.rd_val = reg.pc; // store pc in rd.
                reg.pc = BP.predict() ? BP.jmpc(ret.t2) : reg.pc;
            }
        }
        ret.t1 = IF.fetch(reg.pc, mem);
        return ret;
    }
public:
    CPU(): flag(0) {}
    void init(istream &i) {
        mem.init(i);
    }
    void work() {
        Clock cur;
        memset(cur.vaild, 0, sizeof cur.vaild);
        for(uint clo = 0; ; clo++) {
            cur = stepClock(cur, clo);
            if(flag) break;
        }
        assert(!reg.x[0]);
        printf("%d\n", reg.x[10] & 255);
        BP.print();
    }
};

}
#endif //RISC_V_OTHER_HPP
