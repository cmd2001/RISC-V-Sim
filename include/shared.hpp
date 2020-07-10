#ifndef RISC_V_SHARED_HPP
#define RISC_V_SHARED_HPP

#include <iostream>
#include <cstring>
#include <bitset>
#include <cassert>
#ifndef debug
#define debug cerr
#endif
using namespace std;

namespace RISC_V {
typedef unsigned int uint;
typedef unsigned char uchar;

constexpr uint endIns = 0x0ff00513;

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
    uint rs1, rs2, rd, imm, oriIns;
    Instruction() {ins = INSTRUCTION(0), tpe = INSTRUCTION_TYPE(0), rs1 = rs2 = rd = -1, imm = 0;}
    void print() const {
        const string ls[] = {"LUI","AUIPC","JAL","JALR","BEQ","BNE","BLT","BGE","BLTU","BGEU","LB","LH","LW","LBU","LHU","SB","SH","SW","ADDI","SLTI","SLTIU","XORI","ORI","ANDI","SLLI","SRLI","SRAI","ADD","SUB","SLL","SLT","SLTU","XOR","SRL","SRA","OR","AND"};
        for(int i = 0; i < 37; i++) if(int(ins) == i) debug << ls[i] << " ";
        if(rs1 != uint(-1)) debug << "x" << rs1 << " ";
        if(rs2 != uint(-1)) debug << "x" << rs2 << " ";
        if(rd != uint(-1)) debug << "x" << rd << " ";
        debug << imm << endl;
    }
};

class Registers_Diff {
public:
    uchar rs1_pos, rs2_pos, rd_pos;
    uint rs1_val, rs2_val, rd_val, pc_val;
    bool rd_changed, pc_changed;
    Registers_Diff(uchar _rs1_pos = uchar(-1), uchar _rs2_pos = uchar(-1), uchar _rd_pos = uchar(-1), uint _pc_val = uint(-1)):
        rs1_pos(_rs1_pos), rs2_pos(_rs2_pos), rd_pos(_rd_pos), pc_val(_pc_val), rd_changed(0), pc_changed(0) {}
    void merge(const Registers_Diff &rhs) {
        if(rhs.rd_changed) {
            if(rs1_pos == rhs.rd_pos) rs1_val = rhs.rd_val;
            if(rs2_pos == rhs.rd_pos) rs2_val = rhs.rd_val;
            if(rd_pos == rhs.rd_pos) rd_val = rhs.rd_val;
        }
        if(rhs.pc_changed) pc_val = rhs.pc_val;
    }
};

class Registers {
public:
    uint x[32], pc;
    Registers() { memset(x, 0, sizeof x), pc = 0; }
    void merge(const Registers_Diff &rhs) {
        if(rhs.rd_changed) x[rhs.rd_pos] = rhs.rd_val;
        if(rhs.pc_changed) pc = rhs.pc_val;
    }
    void print() const {
        for(uint i = 0; i < 32; i++) debug << x[i] << " "; debug << pc << endl;
    }

};

constexpr size_t memory_Size = 0x20000;
class Memory {
private:
    unsigned char* dat;
    uint hex2uint(const string &s, uint i = 0) {
        uint ret = 0;
        while(i < s.length()) {
            if('0' <= s[i] && s[i] <= '9') ret = ret * 16 + s[i] - '0';
            else if('A' <= s[i] && s[i] <= 'F') ret = ret * 16 + s[i] - 'A' + 10;
            else debug << "ERROR:: ILLEGAL INPUT IN MEMORY::HEX2UINT()" << endl, assert(0);
            ++i;
        }
        return ret;
    }
public:
    Memory() { dat = new unsigned char[memory_Size], memset(dat, 0, memory_Size); }
    ~Memory() { delete[] dat; }
    unsigned char & operator [] (const uint &p) { return dat[p]; }
    const unsigned char & operator [] (const uint &p) const { return dat[p]; }
    void init(istream &i) {
        uint p = 0;
        string x;
        while(!i.eof()) {
            i >> x;
            if(x[0] == '@') p = hex2uint(x, 1);
            else dat[p++] = hex2uint(x);
        }
    }
};

struct IF2ID { uint ins; };
struct ID2EX { Instruction ins; Registers_Diff reg; };
struct EX2MEM { Instruction ins; Registers_Diff reg; };
struct MEM2WB { Instruction ins; Registers_Diff reg; };

void printBin(uint x) {
    string ret = "";
    for(uint i = 0; i < 32; i++) ret = char('0' + (x & 1)) + ret, x >>= 1;
    debug << ret << endl;
}
void printHex(uint x) {
    string ret = "";
    while(x) {
        uint y = x % 16; x /= 16;
        if(y < 10) ret = char('0' + y) + ret;
        else ret = char('A' + y - 10) + ret;
    }
    while(ret.length() < 8) ret = '0' + ret;
    debug << "0x" + ret << endl;
}

}

#endif //RISC_V_SHARED_HPP
