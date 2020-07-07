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
    void execute(Instruction &ins, Registers &r) {
        if(ins.tpe == LIMM || ins.tpe == OPEI || ins.tpe == OPE) {
            switch(ins.ins) {
                case LUI:
                    r.x[ins.rd] = ins.imm;
                    break;
                case AUIPC: // FIXME: pc - 4?
                    r.x[ins.rd] = ins.imm + r.pc;
                    break;
                case ADDI:
                    r.x[ins.rd] = r.x[ins.rs1] + ins.imm;
                    break;
                case SLTI:
                    r.x[ins.rd] = signed(r.x[ins.rs1]) < signed(ins.imm);
                    break;
                case SLTIU:
                    r.x[ins.rd] = (r.x[ins.rs1] < ins.imm);
                    break;
                case ANDI:
                    r.x[ins.rd] = r.x[ins.rs1] & ins.imm;
                    break;
                case ORI:
                    r.x[ins.rd] = r.x[ins.rs1] | ins.imm;
                    break;
                case XORI:
                    r.x[ins.rd] = r.x[ins.rs1] ^ ins.imm;
                    break;
                case SLLI:
                    r.x[ins.rd] = r.x[ins.rs1] << ins.imm;
                    break;
                case SRLI:
                    r.x[ins.rd] = r.x[ins.rs1] >> ins.imm;
                    break;
                case SRAI: // FIXME: signed?
                    r.x[ins.rd] = signed(r.x[ins.rs1]) >> ins.imm;
                    break;
                case ADD:
                    r.x[ins.rd] = r.x[ins.rs1] + r.x[ins.rs2];
                    break;
                case SUB:
                    r.x[ins.rd] = r.x[ins.rs1] - r.x[ins.rs2];
                    break;
                case SLT:
                    r.x[ins.rd] = signed(r.x[ins.rs1]) < signed(r.x[ins.rs2]);
                    break;
                case SLTU:
                    r.x[ins.rd] = r.x[ins.rs1] < r.x[ins.rs2];
                    break;
                case XOR:
                    r.x[ins.rd] = r.x[ins.rs1] ^ r.x[ins.rs2];
                    break;
                case SLL:
                    r.x[ins.rd] = r.x[ins.rs1] << cutBit(r.x[ins.rs2], 0, 5);
                    break;
                case SRL:
                    r.x[ins.rd] = r.x[ins.rs1] >> cutBit(r.x[ins.rs2], 0, 5);
                    break;
                case SRA:
                    r.x[ins.rd] = signed(r.x[ins.rs1]) >> cutBit(r.x[ins.rs2], 0, 5);
                    break;
                case OR:
                    r.x[ins.rd] = r.x[ins.rs1] | r.x[ins.rs2];
                    break;
                case AND:
                    r.x[ins.rd] = r.x[ins.rs1] & r.x[ins.rs2];
                    break;
                default:
                    debug << "BAD INSTRUCTION IN EXECUTOR.EXECUTE()" << endl;
                    assert(0);
            }
        } else if(ins.tpe == LOAD || ins.tpe == STORE) {
            ins.imm += r.x[ins.rs1];
            assert(uint(LB) <= uint(ins.ins) && uint(ins.ins) <= uint(SW));
        }
    }
};
}

#endif //RISC_V_EXECUTOR_HPP
