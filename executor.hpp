#ifndef RISC_V_EXECUTOR_HPP
#define RISC_V_EXECUTOR_HPP

#include "include/shared.hpp"

namespace RISC_V {
class Executor {
private:
    uint cutBit(const uint &x, const uint &l, const uint &r) {
        return (x & ((1u << r) - (1u << l))) >> l;
    }
public:
    EX2MEM execute(ID2EX arg) {
        Instruction ins = arg.ins;
        Registers_Diff r = arg.reg;
        if(ins.oriIns == endIns) return (EX2MEM){ins, r};

        if(ins.tpe == JMPC) {
            const uint cur = r.pc_val - 4; // this instruction
            switch(ins.ins) {
                case BEQ:
                    if(r.rs1_val == r.rs2_val) r.pc_val = cur + ins.imm, r.pc_changed = 1;
                    break;
                case BNE:
                    if(r.rs1_val != r.rs2_val) r.pc_val = cur + ins.imm, r.pc_changed = 1;
                    break;
                case BLT:
                    if(signed(r.rs1_val) < signed(r.rs2_val)) r.pc_val = cur + ins.imm, r.pc_changed = 1; // not !=
                    break;
                case BLTU:
                    if(r.rs1_val < r.rs2_val) r.pc_val = cur + ins.imm, r.pc_changed = 1;
                    break;
                case BGE:
                    if(signed(r.rs1_val) >= signed(r.rs2_val)) r.pc_val = cur + ins.imm, r.pc_changed = 1;
                    break;
                case BGEU:
                    if(r.rs1_val >= r.rs2_val) r.pc_val = cur + ins.imm, r.pc_changed = 1; // equal !!!!
                    break;
                default:
                    debug << "BAD INSTRUCTION IN DECODER.DECODE()" << endl;
                    assert(0);
            }
            if(r.pc_val & 3) {
                debug << "MISALIGNED INSTRUCTUON DETECTED IN DECODER.DECODE()" << endl;
                assert(0);
            }
        } else if(ins.tpe == LIMM || ins.tpe == OPEI || ins.tpe == OPE) {
            r.rd_changed = 1;
            switch(ins.ins) {
                case LUI:
                    r.rd_val = ins.imm;
                    break;
                case AUIPC:
                    r.rd_val = ins.imm + r.pc_val;
                    break;
                case ADDI:
                    r.rd_val = r.rs1_val + ins.imm;
                    break;
                case SLTI:
                    r.rd_val = signed(r.rs1_val) < signed(ins.imm);
                    break;
                case SLTIU:
                    r.rd_val = (r.rs1_val < ins.imm);
                    break;
                case ANDI:
                    r.rd_val = r.rs1_val & ins.imm;
                    break;
                case ORI:
                    r.rd_val = r.rs1_val | ins.imm;
                    break;
                case XORI:
                    r.rd_val = r.rs1_val ^ ins.imm;
                    break;
                case SLLI:
                    r.rd_val = r.rs1_val << ins.imm;
                    break;
                case SRLI:
                    r.rd_val = r.rs1_val >> ins.imm;
                    break;
                case SRAI:
                    r.rd_val = signed(r.rs1_val) >> ins.imm;
                    break;
                case ADD:
                    r.rd_val = r.rs1_val + r.rs2_val;
                    break;
                case SUB:
                    r.rd_val = r.rs1_val - r.rs2_val;
                    break;
                case SLT:
                    r.rd_val = signed(r.rs1_val) < signed(r.rs2_val);
                    break;
                case SLTU:
                    r.rd_val = r.rs1_val < r.rs2_val;
                    break;
                case SLL:
                    r.rd_val = r.rs1_val << cutBit(r.rs2_val, 0, 5);
                    break;
                case SRL:
                    r.rd_val = r.rs1_val >> cutBit(r.rs2_val, 0, 5);
                    break;
                case SRA:
                    r.rd_val = signed(r.rs1_val) >> cutBit(r.rs2_val, 0, 5);
                    break;
                case OR:
                    r.rd_val = r.rs1_val | r.rs2_val;
                    break;
                case AND:
                    r.rd_val = r.rs1_val & r.rs2_val;
                    break;
                case XOR:
                    r.rd_val = r.rs1_val ^ r.rs2_val;
                    break;
                default:
                    debug << "BAD INSTRUCTION IN EXECUTOR.EXECUTE()" << endl;
                    assert(0);
            }
        } else if(ins.tpe == LOAD || ins.tpe == STORE) {
            ins.imm += r.rs1_val;
            assert(uint(LB) <= uint(ins.ins) && uint(ins.ins) <= uint(SW));
        }
        return (EX2MEM){ins, r};
    }
};
}

#endif //RISC_V_EXECUTOR_HPP
