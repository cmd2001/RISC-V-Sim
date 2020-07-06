#ifndef RISC_V_DECODER_CPP_HPP
#define RISC_V_DECODER_CPP_HPP

typedef unsigned int uint;

namespace RISC_V {

enum INSTRUCTION {
    LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
};
enum INSTRUCTION_TYPE {
    LIMM, // LU, LUIPC
    JMP, // JAL, JALR
    JMPC, // BEQ, BNE, BLT, BGE, BLTU, BGEU
    LOAD, // LB, LH, LW, LBU, LHU
    SAVE, // SB, SH, SW
    OPEI, // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
    OPE, // ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
};

class Instruction {
public:
    const INSTRUCTION ins;
    const INSTRUCTION_TYPE tpe;
    const uint rs1, rs2, imm;
    Instruction(const INSTRUCTION &_ins, const INSTRUCTION_TYPE &_tpe, const uint &_rs1, const uint &_rs2, const uint &_imm): ins(_ins), tpe(_tpe), rs1(_rs1), rs2(_rs2), imm(_imm) {}
};

class Decoder {
public:

};

}
#endif //RISC_V_DECODER_CPP_HPP
