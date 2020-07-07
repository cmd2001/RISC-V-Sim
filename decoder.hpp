#ifndef RISC_V_DECODER_CPP_HPP
#define RISC_V_DECODER_CPP_HPP

#include "include/shared.hpp"

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
            const uint mask = (1u << 32) - (1u << startP);
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
        printBin(ins);
        // printBin(cutBit(ins, 0, 7));
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
                printBin(ret.imm);
                break;
            case 103: //1100111, JALR
                ret.ins = JALR, ret.tpe = JMP;
                ret.rd = cutBit(ins, 7, 12);
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
                debug << "step = " << step << endl;
                step2 = getBit(ins, 30);
                if(!step) ret.ins = INSTRUCTION(int(ADD) + step2);
                else if(step <= 4) ret.ins = INSTRUCTION(int(SLL) + step - 1);
                else if(step == 5) ret.ins = INSTRUCTION(int(SRL) + step2);
                else ret.ins = INSTRUCTION(int(OR) + step - 6);
                break;
            default:
                debug << "ERROR:: UNKNOWN INSTRUCTION" << endl;
                assert(0);
        }
        return ret;
    }
};

class Decoder {
private:
    Instruction_Decoder id;
public:
    Instruction decode(const uint &_ins, Registers &r) { // decode instruction and solve jump.
        Instruction ins = id.decode(_ins);
        if(ins.tpe == JMP || ins.tpe == JMPC) {
            r.pc -= 4; // FIXME: displace effect in IR
            switch(ins.ins) {
                case JAL:
                    r.x[ins.rd] = r.pc + 4;
                    r.pc += ins.imm;
                    break;
                case JALR:
                    r.x[ins.rd] = r.pc + 4;
                    r.pc += ins.imm + r.x[ins.rs1];
                    break;
                case BEQ:
                    if(r.x[ins.rs1] == r.x[ins.rs2]) r.pc += ins.imm;
                    break;
                case BNE:
                    if(r.x[ins.rs1] != r.x[ins.rs2]) r.pc += ins.imm;
                    break;
                case BLT:
                    if(signed(r.x[ins.rs1]) != signed(r.x[ins.rs2])) r.pc += ins.imm;
                    break;
                case BLTU:
                    if(r.x[ins.rs1] < r.x[ins.rs2]) r.pc += ins.imm;
                    break;
                case BGE:
                    if(signed(r.x[ins.rs1]) >= signed(r.x[ins.rs2])) r.pc += ins.imm;
                    break;
                case BGEU:
                    if(r.x[ins.rs1] > r.x[ins.rs2]) r.pc += ins.imm;
                    break;
                default:
                    debug << "BAD INSTRUCTION IN DECODER.DECODE()" << endl;
                    assert(0);
            }
        }
        return ins;
    }
};

}
#endif //RISC_V_DECODER_CPP_HPP
