#ifndef RISC_V_DECODER_CPP_HPP
#define RISC_V_DECODER_CPP_HPP

#include "../include/shared.hpp"

namespace RISC_V {

class Instruction_Decoder {
private:
    uint cutBit(const uint &x, const uint &l, const uint &r) {
        return (x & ((1u << r) - (1u << l))) >> l;
    }
    uint getBit(const uint &x, const uint &p) {
        return (x >> p) &1u;
    }
    void writeBit(uint &dst, const uint &p, const uint &x) {
        dst |= (x << p);
    }
    void writeSgn(uint &dst, const uint &startP, const uint &sgn) {
        if(sgn) {
            const uint mask = - (1u << startP);
            dst |= mask;
        }
    }
    uint imm_I(const uint &x) {
        uint ret = 0;
        writeBit(ret, 0, getBit(x, 20));
        writeBit(ret, 1, cutBit(x, 21, 31));
        const uint sgn = getBit(x, 31);
        writeSgn(ret, 11, sgn);
        return ret;
    }
    uint imm_S(const uint &x) {
        uint ret = 0;
        writeBit(ret, 0, getBit(x, 7));
        writeBit(ret, 1, cutBit(x, 8, 12));
        writeBit(ret, 5, cutBit(x, 25, 31));
        const uint sgn = getBit(x, 31);
        writeSgn(ret, 11, sgn);
        return ret;
    }
    uint imm_B(const uint &x) {
        uint ret = 0;
        writeBit(ret, 1, cutBit(x, 8, 12));
        writeBit(ret, 5, cutBit(x, 25, 31));
        writeBit(ret, 11, getBit(x, 7));
        const int sgn = getBit(x, 31);
        writeSgn(ret, 12, sgn);
        return ret;
    }
    uint imm_U(const uint &x) {
        uint ret = 0;
        writeBit(ret, 12, cutBit(x, 12, 32));
        return ret;
    }
    uint imm_J(const uint &x) {
        uint ret = 0;
        writeBit(ret, 1, cutBit(x, 21, 31));
        writeBit(ret, 11, getBit(x, 20));
        writeBit(ret, 12, cutBit(x, 12, 20));
        const uint sgn = getBit(x, 31);
        writeSgn(ret, 20, sgn);
        return ret;
    }
public:
    Instruction decode(const uint &ins) {
        Instruction ret;
        uint step, step2; // FIX variable redefinition in switch
        ret.oriIns = ins;
        switch(cutBit(ins, 0, 7)) {
            case 55: // 0110111, LUI
                ret.ins = LUI, ret.tpe = LIMM;
                ret.rd = cutBit(ins, 7, 12);
                ret.imm = imm_U(ins);
                break;
            case 23: // 0010111, AUIPC
                ret.ins = AUIPC, ret.tpe = LIMM;
                ret.rd = cutBit(ins, 7, 12);
                ret.imm = imm_U(ins);
                break;
            case 111: //1101111, JAL
                ret.ins = JAL, ret.tpe = JMP;
                ret.rd = cutBit(ins, 7, 12);
                ret.imm = imm_J(ins);
                break;
            case 103: //1100111, JALR
                ret.ins = JALR, ret.tpe = JMP;
                ret.rd = cutBit(ins, 7, 12);
                ret.rs1 = cutBit(ins, 15, 20);
                ret.imm = imm_I(ins);
                break;
            case 99: // 1100011
                ret.tpe = JMPC;
                ret.rs1 = cutBit(ins, 15, 20);
                ret.rs2 = cutBit(ins, 20, 25);
                ret.imm = imm_B(ins);
                step = cutBit(ins, 12, 15);
                if(step < 4) ret.ins = INSTRUCTION(int(BEQ) + step); // WHAT A FUCK IS THIS?
                else ret.ins = INSTRUCTION(int(BLT) + step - 4);
                break;
            case 3: //  0000011, for LB...LHU
                ret.tpe = LOAD;
                ret.rd = cutBit(ins, 7, 12);
                ret.rs1 = cutBit(ins, 15, 20);
                ret.imm = imm_I(ins);
                step = cutBit(ins, 12, 15);
                if(step < 4) ret.ins = INSTRUCTION(int(LB) + step);
                else ret.ins = ret.ins = INSTRUCTION(int(LBU) + step - 4);
                break;
            case 35: // 0100011, for SB...SW
                ret.tpe = STORE;
                ret.rs1 = cutBit(ins, 15, 20);
                ret.rs2 = cutBit(ins, 20, 25);
                ret.imm = imm_S(ins);
                step = cutBit(ins, 12, 15);
                ret.ins = INSTRUCTION(int(SB) + step);
                break;
            case 19: // 0010011, for ADDI...ANDI...SRAI
                ret.tpe = OPEI;
                ret.rd = cutBit(ins, 7, 12);
                ret.rs1 = cutBit(ins, 15, 20);
                step = cutBit(ins, 12, 15);
                if(step == 1 || step == 5) {
                    ret.imm = cutBit(ins, 20, 25);
                    step2 = getBit(ins, 30);
                    if(step == 1) ret.ins = SLLI;
                    else if(step == 5) ret.ins = INSTRUCTION(int(SRLI) + step2);
                } else {
                    ret.imm = imm_I(ins);
                    if (!step) ret.ins = ADDI;
                    else if(step < 4) ret.ins = INSTRUCTION(int(SLTI) + step - 2);
                    else if(step == 4) ret.ins = XORI;
                    else ret.ins = INSTRUCTION(int(ORI) + step - 6);
                }
                break;
            case 51: // 0110011, ADD...AND
                ret.tpe = OPE;
                ret.tpe = OPE;
                ret.rd = cutBit(ins, 7, 12);
                ret.rs1 = cutBit(ins, 15, 20);
                ret.rs2 = cutBit(ins, 20, 25);
                step = cutBit(ins, 12, 15);
                // debug << "step = " << step << endl;
                step2 = getBit(ins, 30);
                if(!step) ret.ins = INSTRUCTION(int(ADD) + step2);
                else if(step <= 4) ret.ins = INSTRUCTION(int(SLL) + step - 1);
                else if(step == 5) ret.ins = INSTRUCTION(int(SRL) + step2);
                else ret.ins = INSTRUCTION(int(OR) + step - 6);
                break;
            default:
                debug << "ERROR:: UNKNOWN INSTRUCTION " << ins << endl;
                assert(0);
        }
        return ret;
    }
};

class Decoder {
private:
    Instruction_Decoder id;
public:
    ID2EX decode(const IF2ID &__ins, const Registers &r) { // decode instruction and solve jump.
        const uint _ins = __ins.ins;
        Instruction ins = id.decode(_ins);
        ins.insPC = __ins.pc;
        Registers_Diff ret = Registers_Diff(ins.rs1, ins.rs2, ins.rd, r.pc);
        if(~ins.rs1) ret.rs1_val = r.x[ins.rs1];
        if(~ins.rs2) ret.rs2_val = r.x[ins.rs2];
        if(~ins.rd) ret.rd_val = r.x[ins.rd];
        return (ID2EX){ins, ret};
    }
    uint jmp(ID2EX &arg) {
        const Instruction &ins = arg.ins;
        Registers_Diff &r = arg.reg;
        const uint cur = r.pc_val - 4;
        switch (ins.ins) {
            case JAL:
                if(ins.rd) r.rd_val = cur + 4, r.rd_changed = 1;
                r.pc_val = cur + ins.imm, r.pc_changed = 1;
                break;
            case JALR:
                if(ins.rd) r.rd_val = cur + 4, r.rd_changed = 1;
                r.pc_val = (ins.imm + r.rs1_val) & (unsigned(-2)), r.pc_changed = 1; // instead of +=, ignore lowest bit.
                break;
        }
        return r.pc_val;
    }
};

}
#endif //RISC_V_DECODER_CPP_HPP
