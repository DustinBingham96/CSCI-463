#include "rv32i.h"
#include "memory.h"
#include "hex.h"
#include <iostream>
#include <iomanip>
#include<string>
using namespace std;


/**
 *  Save the m argument in the mem member variable for use later when disassembling
 *   
 *
 *   
 ********************************************************************************/
rv32i::rv32i(memory *m)
{
    mem = m;
}



/**
 *  This method will be used to disassemble the instructions in the simulated memory
 *    
 *
 *   
 ********************************************************************************/
void rv32i::disasm(void)
{
    pc = 0;
    uint32_t insn = 0;

    while(pc < mem->get_size())
    {
        cout << hex << setfill('0') << setw(8) << pc << ": ";   //print the 32bit hex address in the pc register
        insn = mem->get32(pc);                                  //fetch the 32-bit instruction from memory at the address in the pc register
        cout << hex << setfill('0') << setw(8) << insn << "  "; //print the instruction as a 32-bit hex value
        cout<<decode(insn)<<endl;                               //pass the fetched instruction to decode() and print
        pc = pc + 4;                                            //increment pc by 4
    }
}
    
/**
 *  It is the purpose of this function to return a std::string containing the disassembled instruction text.
 *     
 *
 * 
 * @param insn
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::decode(uint32_t insn) const
{
    uint32_t opcode = get_opcode(insn);
    uint32_t funct3 = get_funct3(insn);
    uint32_t funct7 = get_funct7(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t suc = (insn & 0x00F00000) >> 20; //used for ecall and ebreak

    switch(opcode)
    {
        default:            return render_illegal_insn();
        
        //U Type
        case 0b0110111:     return render_lui(insn);                    //case: lui
        case 0b0010111:     return render_auipc(insn);                  //case: auipc
        //R Type
        case 0b0110011:
                switch(funct3)
                {
                    default:    return render_illegal_insn();
                    case 0b000:                                         //case: add or sub
                        switch(funct7)
                        {
                            default:            return render_illegal_insn();
                            case 0b0000000:     return render_rtype(insn, "add");
                            case 0b0100000:     return render_rtype(insn, "sub");
                        }
                    case 0b001:     return render_rtype(insn, "sll");   //case: sll
                    case 0b010:     return render_rtype(insn, "slt");   //case: slt
                    case 0b011:     return render_rtype(insn, "sltu");  //case: sltu
                    case 0b100:     return render_rtype(insn, "xor");   //case: xor
                    case 0b101:                                         //case: srl or sra
                        switch(funct7)
                        {
                            default:            return render_illegal_insn();
                            case 0b0000000:     return render_rtype(insn, "srl");
                            case 0b0100000:     return render_rtype(insn, "sra");
                        }
                    case 0b110:     return render_rtype(insn, "or");    //case: or
                    case 0b111:     return render_rtype(insn, "and");   //case: and
                }
        //S Type
        case 0b0100011:
                switch(funct3)
                {
                    default:        return render_illegal_insn();
                    case 0b000:     return render_stype(insn, "sb");
                    case 0b001:     return render_stype(insn, "sh");
                    case 0b010:     return render_stype(insn, "sw");
                }
        //I Type
        //alu
        case 0b0010011:
                switch(funct3)
                {
                    default:        return render_illegal_insn();
                    case 0b000:     return render_itype_alu(insn, "addi", imm_i);
                    case 0b010:     return render_itype_alu(insn, "slti", imm_i);
                    case 0b011:     return render_itype_alu(insn, "sltiu", imm_i);
                    case 0b100:     return render_itype_alu(insn, "xori", imm_i);
                    case 0b110:     return render_itype_alu(insn, "ori", imm_i);
                    case 0b111:     return render_itype_alu(insn, "andi", imm_i%XLEN);
                    case 0b001:     return render_itype_alu(insn, "slli", imm_i%XLEN);
                    case 0b101:
                            switch(funct7)
                            {
                                default:            return render_illegal_insn();
                                case 0b0000000:     return render_itype_alu(insn, "srli", imm_i);
                                case 0b0100000:     return render_itype_alu(insn, "srai", imm_i%XLEN);
                            }
                }
        //load
        case 0b0000011:
                switch(funct3)
                {
                    default:        return render_illegal_insn();
                    case 0b000:     return render_itype_load(insn, "lb");
                    case 0b001:     return render_itype_load(insn, "lh");
                    case 0b010:     return render_itype_load(insn, "lw");
                    case 0b100:     return render_itype_load(insn, "lbu");
                    case 0b101:     return render_itype_load(insn, "lhu");
                }
        //B Type
        case 0b1100011:
                switch(funct3)
                {
                    default:        return render_illegal_insn();
                    case 0b000:     return render_btype(insn, "beq");
                    case 0b001:     return render_btype(insn, "bne");
                    case 0b100:     return render_btype(insn, "blt");
                    case 0b101:     return render_btype(insn, "bge");
                    case 0b110:     return render_btype(insn, "bltu");
                    case 0b111:     return render_btype(insn, "bgeu");
                }
        //J Type
        case 0b1101111:     return render_jal(insn);
        case 0b1100111:     return render_jalr(insn);

        //fence
        case 0b0001111:     return render_fence(insn);
        //ecall and ebreak
        case 0b1110011:
                switch(suc)
                {
                    default:        return render_illegal_insn();
                    case 0b000000000000:    return render_ecall();
                    case 0b000000000001:    return render_ebreak();
                }
        
    }
}

/**
 *  Extract and return the opcode field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return uint32_t opcode
 *   
 ********************************************************************************/
uint32_t rv32i::get_opcode(uint32_t insn) 
{
    return (insn & 0x0000007f);
}

/**
 *  Extract and return the imm_u field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return uint32_t imm_u
 *   
 ********************************************************************************/
int32_t rv32i::get_imm_u(uint32_t insn)
{
    return (insn & 0xfffff000);
}

/**
 *  Extract and return the rd field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return uint32_t rd
 *   
 ********************************************************************************/
uint32_t rv32i::get_rd(uint32_t insn)
{
    return ((insn & 0x00000f80)>>7);
}

/**
 *  Extract and return the funct3 field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return uint32_t funct3
 *   
 ********************************************************************************/
uint32_t rv32i::get_funct3(uint32_t insn)
{
    return ((insn & 0x00007000)>>12);
}

/**
 *  Extract and return the rs1 field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return uint32_t rs1
 *   
 ********************************************************************************/
uint32_t rv32i::get_rs1(uint32_t insn)
{
    return ((insn & 0x000f8000)>>15);
}

/**
 *  Extract and return the rs2 field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return uint32_t rs2
 *   
 ********************************************************************************/
uint32_t rv32i::get_rs2(uint32_t insn)
{
    return ((insn & 0x01f00000)>>20);
}

/**
 *  Extract and return the funct7 field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return uint32_t funct7
 *   
 ********************************************************************************/
uint32_t rv32i::get_funct7(uint32_t insn)
{
    return ((insn & 0xfe000000)>>25);
}

/**
 *  Extract and return the imm_s field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return int32_t imm_s
 *   
 ********************************************************************************/
int32_t rv32i::get_imm_s(uint32_t insn)
{
    //extract imm_s
    int32_t imm_s = (insn & 0xfe000000) >> (25 - 5);
    imm_s |= (insn & 0x00000f80) >> (7 - 0);

    //sign extend
    if (insn & 0x80000000)
        imm_s |= 0xfffff000;
    
    return imm_s;
}

/**
 *  Extract and return the imm_i field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return int32_t imm_i
 *   
 ********************************************************************************/
int32_t rv32i::get_imm_i(uint32_t insn)
{
    int32_t imm_i = ((insn & 0xfff00000)>>20);

    //sign extend
    if (insn & 0x80000000)
        imm_i |= 0xfffff000;

    return imm_i;
}

/**
 *  Extract and return the imm_b field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return int32_t imm_b
 *   
 ********************************************************************************/

int32_t rv32i::get_imm_b(uint32_t insn)
{
    //extract imm_b
    int32_t imm_b = (insn & 0x80000000) >> (31-12);
    imm_b |= (insn & 0x7e000000) >> (25-5);
    imm_b |= (insn & 0x00000f00) >> (8-1);
    imm_b |= (insn & 0x00000080) << (11-7);

    //sign extend
    if (insn & 0x80000000)
        imm_b |= 0xfffff000;
    
    return imm_b;
}

/**
 *  Extract and return the imm_j field from the given instruction
 *     
 *
 * 
 * @param insn
 * @return int32_t imm_j
 *   
 ********************************************************************************/
int32_t rv32i::get_imm_j(uint32_t insn)
{
    //extract imm_j
    int32_t imm_j = (insn & 0x000ff000);
    imm_j |= (insn & 0x00100000) >> (20-11);
    imm_j |= (insn & 0x7fe00000) >> (30-10);
    imm_j |= (insn & 0x80000000) >> (31-20);

    //sign extend
    if (insn & 0x80000000)
        imm_j |= 0xfff00000;
    
    return imm_j;

}

/**
 *  Default render method for decode
 *     
 *
 * 
 * @param n/a
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_illegal_insn() const
{
    return ("ERROR: UNIMPLEMENTED INSTRUCTION");
}

/**
 *  lui render method
 *     
 *
 * 
 * @param insn
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_lui(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << "lui" << "x" << dec << rd << ",0x" << hex <<((imm_u >> 12)&0x0fffff);
    return os.str();
}

/**
 *  auipc render method
 *     
 *
 * 
 * @param insn
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_auipc(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << "auipc" << "x" << dec << rd << ",0x" << hex <<((imm_u >> 12)&0x0fffff);
    return os.str();
}

/**
 *  rtype render method
 *     
 *
 * 
 * @param insn
 * @param mnemonic
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_rtype(uint32_t insn, const char *mnemonic) const
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << mnemonic << "x" << dec << rd << ",x" << rs1 << ",x" << rs2;
    return os.str();

}

/**
 *  stype render method
 *     
 *
 * 
 * @param insn
 * @param mnemonic
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_stype(uint32_t insn, const char *mnemonic) const
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << mnemonic << "x" << dec << rs2 << "," << imm_s << "(x" << rs1 << ")";
    return os.str();
}

/**
 *  itype alu render method
 *     
 *
 * 
 * @param insn
 * @param mnemonic
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i) const
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << mnemonic << "x" << dec << rd << ",x" << rs1 << "," << imm_i;
    return os.str();

}

/**
 *  itype load render method
 *     
 *
 * 
 * @param insn
 * @param mnemonic
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_itype_load(uint32_t insn, const char *mnemonic) const
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << mnemonic << "x" << dec << rd << "," << imm_i << "(x" << rs1 << ")";
    return os.str();
}

/**
 *  btype render method
 *     
 *
 * 
 * @param insn
 * @param mnemonic
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_btype(uint32_t insn, const char *mnemonic) const
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t pcrel_13 = get_imm_b(insn) + pc;

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << mnemonic << "x" << dec << rs1 << ",x" << rs2 << ",0x" << hex << pcrel_13;
    return os.str();

}

/**
 *  jal render method
 *     
 *
 * 
 * @param insn
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_jal(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    int32_t pcrel_21 = get_imm_j(insn) + pc;

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << "jal" << "x" << dec << rd << ",0x" << hex << pcrel_21;
    return os.str();
}

/**
 *  jalr render method
 *     
 *
 * 
 * @param insn
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_jalr(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << "jalr" << "x" << dec << rd << "," << imm_i << "(x" << rs1 << ")"; 
    return os.str();

}

/**
 *  fence render method
 *     
 *
 * 
 * @param insn
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_fence(uint32_t insn) const
{
    std::ostringstream os;
    os << std::setw(mnemonic_width) << setfill(' ') << left << "fence"
        << (insn & 0x08000000 ? "i" : "") << (insn & 0x04000000 ? "o" : "") << (insn & 0x02000000 ? "r" : "") << (insn & 0x01000000 ? "w" : "") << ","
        << (insn & 0x00800000 ? "i" : "") << (insn & 0x00400000 ? "o" : "") << (insn & 0x00200000 ? "r" : "") << (insn & 0x00100000 ? "w" : "");

    return os.str();
}

/**
 *  ecall render method
 *     
 *
 * 
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_ecall() const
{
    return ("ecall");
}

/**
 *  ebreak render method
 *     
 *
 * 
 * @return string
 *   
 ********************************************************************************/
std::string rv32i::render_ebreak() const
{
    return ("ebreak");
}

