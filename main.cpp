#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>

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
Registers reg, reg2;


int main() {
    mem.init(cin);
    uint _ins;
    Instruction ins;
    while(1) {
        _ins = IF.fetch(reg, mem); // fetch instruction and add PC.
        ins = ID.decode(_ins, reg2, reg); // read registers, decode instruction and apply compare and jump.
        EX.execute(ins, reg2); // do ALU operates and calculate address, which is stored in imm.
        MEM.work(ins, reg2, mem); //
        WB.writeBack(reg, reg2); // write back registers.
    }
    return 0;
}
