#ifndef RISC_V_SHARED_HPP
#define RISC_V_SHARED_HPP

#include <iostream>
#include <cassert>
#ifndef debug
#define debug cerr
#endif
using namespace std;

namespace RISC_V {
enum INSTRUCTION {
    LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
};

enum INSTRUCTION_TYPE {
    LIMM, // LUI, LUIPC: U-type
    JMP, // JAL, JALR: J-type, I-type
    JMPC, // BEQ, BNE, BLT, BGE, BLTU, BGEU: B-type
    LOAD, // LB, LH, LW, LBU, LHU: I-type
    STORE, // SB, SH, SW: S-type
    OPEI, // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI: I-type
    OPE, // ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND: R-type
};

class Instruction {
public:
    INSTRUCTION ins;
    INSTRUCTION_TYPE tpe;
    uint rs1, rs2, rd, imm;
    Instruction() {ins = INSTRUCTION(0), tpe = INSTRUCTION_TYPE(0), rs1 = rs2 = rd = -1, imm = 0;}
    Instruction(const INSTRUCTION &_ins, const INSTRUCTION_TYPE &_tpe, const uint &_rs1, const uint &_rs2, const uint &_imm): ins(_ins), tpe(_tpe), rs1(_rs1), rs2(_rs2), imm(_imm) {}
    void print() const {
        const string ls[] = {"LUI","AUIPC","JAL","JALR","BEQ","BNE","BLT","BGE","BLTU","BGEU","LB","LH","LW","LBU","LHU","SB","SH","SW","ADDI","SLTI","SLTIU","XORI","ORI","ANDI","SLLI","SRLI","SRAI","ADD","SUB","SLL","SLT","SLTU","XOR","SRL","SRA","OR","AND"};
        for(int i = 0; i < 37; i++) if(int(ins) == i) debug << ls[i] << " ";
        if(rs1 != -1) debug << "x" << rs1 << " ";
        if(rs2 != -1) debug << "x" << rs2 << " ";
        if(rd != -1) debug << "x" << rd << " ";
        debug << imm << endl;
    }
};

void printBin(uint x) {
    string ret = "";
    for(uint i = 0; i < 32; i++) ret = char('0' + (x & 1)) + ret, x >>= 1;
    debug << ret << endl;
}

}

#endif //RISC_V_SHARED_HPP
