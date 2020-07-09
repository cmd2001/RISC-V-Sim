#include "include/shared.hpp"
#include "decoder.hpp"
#include "executor.hpp"
#include "mem.hpp"
#include "other.hpp"

using namespace RISC_V;


Instruction_Fetcher IF;
Decoder ID;
Executor EX;
Memory_Access MEM;
WriteBacker WB;

Memory mem;
Registers reg;

struct Clock {
    IF2ID t1; ID2EX t2; EX2MEM t3; MEM2WB t4;
    Registers reg_New;
    bool vaild[4];
};

uint flag = 0;

Clock stepClock(Clock &c, const uint clo) { // move 1 clock forward.
    static uint lastMem = uint(-1);
    Clock ret;
    for(uint i = 1; i < 4; i++) ret.vaild[i] = c.vaild[i - 1];
    ret.vaild[0] = 1;
    bool branch = 0;
    if(c.vaild[3]) WB.writeBack(reg, c.t4);
    if(c.vaild[2]) {
        if(MEM.check(c.t3)) {
            if(lastMem == uint(-1)) lastMem = clo;
            if(clo != lastMem + 3) { // not enough delay
                ret.t3 = c.t3, ret.t2 = c.t2, ret.t1 = c.t1;
                ret.vaild[3] = 0;
                return ret;
            } else {
                lastMem = uint(-1);
                if(c.t3.ins.tpe == LOAD) { // stall 1 clock
                    ret.t4 = MEM.work(c.t3, mem);
                    ret.t2 = c.t2, ret.t1 = c.t1, ret.vaild[2] = 0;
                    ret.reg_New = ret.t4.reg; // forwarding: short path
                    return ret;
                } else ret.t4 = MEM.work(c.t3, mem); // STORE.
            }
        } else ret.t4 = MEM.work(c.t3, mem);
        if(c.t3.ins.tpe == JMP || c.t3.ins.tpe == JMPC) branch = 1;
    }
    if(c.vaild[1]) {
        c.t2.reg.merge(c.reg_New); // forwarding: update
        ret.t3 = EX.execute(c.t2);
        ret.reg_New = ret.t3.reg;
        if(c.t2.ins.tpe == JMP || c.t2.ins.tpe == JMPC) branch = 1;
    }
    if(c.vaild[0]) {
        ret.t2 = ID.decode(c.t1, reg);
        ret.t2.reg.merge(c.reg_New); // forwarding: update
        if(ret.t2.ins.tpe == JMP || ret.t2.ins.tpe == JMPC) branch = 1;
    }
    if(!branch && !flag) {
        ret.t1 = IF.fetch(reg.pc, mem);
        if(ret.t1.ins == endIns) flag = 1, ret.t1.ins = 19;
    } else ret.t1.ins = 19; // NOP
    return ret;
}

int main() {
    mem.init(cin);
    Clock cur;
    memset(cur.vaild, 0, sizeof cur.vaild);
    for(uint clo = 0; ; clo++) {
        auto tmp = stepClock(cur, clo);
        cur = tmp;
        if(flag && ++flag > 10) break;
    }
    assert(!reg.x[0]);
    printf("%d\n", reg.x[10] & 255);
    return 0;
}
