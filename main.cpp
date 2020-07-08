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
Registers reg, reg2;


int main() {
    mem.init(cin);
    uint _ins;
    Instruction ins;
    int cnt = 0;
    while(1) {
        // debug << "init pc = " << hex << reg.pc << dec << endl;
        _ins = IF.fetch(reg, mem); // fetch instruction and add PC.
        // debug << uint(mem[4491]) << " " << uint(mem[4490]) << " " << uint(mem[4489]) << " " << uint(mem[4488]) << endl;
        // printHex(_ins);
        // if(_ins == 0x00051c63) debug << "push stack" << ++cnt << endl;
        if(_ins == endIns) break;
        ins = ID.decode(_ins, reg2, reg); // read registers, decode instruction and apply compare and jump.
        /* if(ins.tpe == LOAD || ins.tpe == STORE) {
            debug << "PC = " << hex << reg.pc - 4 << dec << endl;
            debug << "_ins = " << hex << _ins << dec << endl;
            ins.print();
            for(int i = 0; i < 32; i++) debug << reg.x[i] << " "; debug << endl;
        } */
        /* if(_ins == 0x00051c63) {
            ins.print(), debug << reg.x[10] << " " << reg.x[0] << endl;
            exit(0);
        } */
        EX.execute(ins, reg2); // do ALU operates and calculate address, which is stored in imm.
        MEM.work(ins, reg2, mem); //
        WB.writeBack(reg, reg2); // write back registers.
    }
    debug << "reg0 = " << reg.x[0] << endl;
    printf("%d\n", reg.x[10] & 255);
    return 0;
}
// basic ops WA
// pi TLE