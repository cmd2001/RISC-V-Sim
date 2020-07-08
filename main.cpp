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
    while(1) {
        IF2ID t1 = IF.fetch(reg.pc, mem); // fetch instruction and add PC.
        if(t1.ins == endIns) break;
        ID2EX t2 = ID.decode(t1, reg); // read registers, decode instruction and apply compare and jump.
        EX2MEM t3 = EX.execute(t2); // do ALU operates and calculate address, which is stored in imm.
        MEM2WB t4 = MEM.work(t3, mem); //
        WB.writeBack(reg, t4); // write back registers.
    }
    assert(!reg.x[0]);
    printf("%d\n", reg.x[10] & 255);
    return 0;
}
