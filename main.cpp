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


int main() {
    mem.init(cin);
    IF2ID t1;
    ID2EX t2;
    EX2MEM t3;
    MEM2WB t4;
    for(uint clo = 0, sta = 0, tp = uint(-1);; clo++) {
        if(sta == 0) {
            t1 = IF.fetch(reg.pc, mem), sta = (sta + 1) % 5;
            if(t1.ins == endIns) break;
        } else if(sta == 1) t2 = ID.decode(t1, reg), sta = (sta + 1) % 5;
        else if (sta == 2) t3 = EX.execute(t2), sta = (sta + 1) % 5;
        else if(sta == 3) {
            if(!MEM.check(t3)) {t4 = (MEM2WB){t3.reg}, sta = (sta + 1) % 5; continue;} // no memory operation, jump to next clock.
            if(tp == uint(-1)) tp = clo; // start waiting.
            if(tp != uint(-1) && clo == tp + 3) t4 = MEM.work(t3, mem), sta = (sta + 1) % 5, tp = uint(-1); // wait completed.
        } else if(sta == 4) WB.writeBack(reg, t4), sta = (sta + 1) % 5;
    }
    assert(!reg.x[0]);
    printf("%d\n", reg.x[10] & 255);
    return 0;
}
