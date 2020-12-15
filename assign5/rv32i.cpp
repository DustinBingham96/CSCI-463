//******************************************************************
//
// CSCI 463 Assignment 5
//
// Author: Dustin Bingham
//
// RISC-V Simulator
//
//******************************************************************
#include "rv32i.h"
#include "memory.h"
#include "hex.h"
#include "registerfile.h"
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
                    case 0b111:     return render_itype_alu(insn, "andi", imm_i);
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

/**
 *  Mutator for show_instructions.
 *     
 *
 * 
 * @param b
 * @return void
 *   
 ********************************************************************************/
void rv32i::set_show_instructions(bool b)
{
    show_instructions = b;
}

/**
 *  Mutator for show_registers.
 *     
 *
 * 
 * @param b
 * @return void
 *   
 ********************************************************************************/
void rv32i::set_show_registers(bool b)
{
    show_registers = b;
}

/**
 *  Accessor for halt.
 *     
 *
 * 
 * @param 
 * @return bool
 *   
 ********************************************************************************/
bool rv32i::is_halted() const
{
    return halt;
}

/**
 *  Reset the rv32i object and the registerfile.
 *     
 *
 * 
 * @param 
 * @return void
 *   
 ********************************************************************************/
void rv32i::reset()
{
    pc = 0;
    insn_counter = 0;
    halt = false;
    regs.reset();
}

/**
 *   dump the state of the hart.
 *     
 *
 * 
 * @param 
 * @return void
 *   
 ********************************************************************************/
void rv32i::dump() const
{
    regs.dump();
    std::cout << " pc " << std::hex << std::setfill('0') << std::setw(8) << pc << std::endl; 
}

/**
 *   If the halt flag is set then return immediately without doing anything. Otherwise, simulate the
 *   execution of one single instruction.
 *     
 *
 * 
 * @param 
 * @return void
 *   
 ********************************************************************************/
void rv32i::tick()
{
    if (halt == true)
    {
        //do nothing
    }
    else
    {
        insn_counter = insn_counter + 1;
        if(show_registers == true)
        {
            regs.dump();
            std::cout << " pc " << std::hex << std::setfill('0') << std::setw(8) << pc << std::endl; 
        }
        uint32_t insn = mem->get32(pc);
        if(show_instructions == true)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(8) << pc << ": " << std::setw(8) << insn << " ";
            dcex(insn, &std::cout);
        }
        else
        {
            dcex(insn, nullptr);
        }   
    }
}

/**  
 *   run method: see handout for more details
 *     
 *
 * 
 * @param limit
 * @return void
 *   
 ********************************************************************************/
void rv32i::run(uint64_t limit)
{
    regs.set(2,mem->get_size());
    while((halt != true && insn_counter <= limit) || (halt != true && limit == 0))
    {
        tick();
    }
    std::cout << "Execution terminated by EBREAK instruction" << std::endl;
    std::cout << std::dec << insn_counter << " instructions executed" << std::endl;

}

/**
 *   This function will execute the given RV32I instruction by making use of the get_xxx() methods to
 *   extract the needed instruction fields to decode the instruction and invoke the associated exec_xxx()
 *   helper function using the same sort of switch-logic from assignment 4
 * 
 *   This function must be capable of handling any 32-bit value. If an illegal instruction is encountered 
 *   then call an exec_illegal_insn() method to take care of the situation.
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::dcex(uint32_t insn, std::ostream* pos)
{
    uint32_t opcode = get_opcode(insn);
    uint32_t funct3 = get_funct3(insn);
    uint32_t funct7 = get_funct7(insn);
    //int32_t imm_i = get_imm_i(insn);
    int32_t suc = (insn & 0x00F00000) >> 20; //used for ecall and ebreak

    switch(opcode)
    {
        default:            exec_illegal_insn(insn, pos); return;
        
        //U Type
        case 0b0110111:     exec_lui(insn, pos); return;                    //case: lui
        case 0b0010111:     exec_auipc(insn, pos); return;                  //case: auipc
        //R Type
        case 0b0110011:
                switch(funct3)
                {
                    default:    exec_illegal_insn(insn, pos);
                    case 0b000:                                         //case: add or sub
                        switch(funct7)
                        {
                            default:            exec_illegal_insn(insn, pos); return;
                            case 0b0000000:     exec_add(insn, pos); return;
                            case 0b0100000:     exec_sub(insn, pos); return;
                        }
                    case 0b001:     exec_sll(insn, pos); return;   //case: sll
                    case 0b010:     exec_slt(insn, pos); return;   //case: slt
                    case 0b011:     exec_sltu(insn, pos); return;  //case: sltu
                    case 0b100:     exec_xor(insn, pos); return;   //case: xor
                    case 0b101:                                         //case: srl or sra
                        switch(funct7)
                        {
                            default:            exec_illegal_insn(insn, pos); return;
                            case 0b0000000:     exec_srl(insn, pos); return;
                            case 0b0100000:     exec_sra(insn, pos); return;
                        }
                    case 0b110:     exec_or(insn, pos); return;    //case: or
                    case 0b111:     exec_and(insn, pos); return;   //case: and
                }
        //S Type
        case 0b0100011:
                switch(funct3)
                {
                    default:        exec_illegal_insn(insn, pos); return;
                    case 0b000:     exec_sb(insn, pos); return;
                    case 0b001:     exec_sh(insn, pos); return;
                    case 0b010:     exec_sw(insn, pos); return;
                }
        //I Type
        //alu
        case 0b0010011:
                switch(funct3)
                {
                    default:        exec_illegal_insn(insn, pos); return;
                    case 0b000:     exec_addi(insn, pos); return;
                    case 0b010:     exec_slti(insn, pos); return;
                    case 0b011:     exec_sltiu(insn, pos); return;
                    case 0b100:     exec_xori(insn, pos); return;
                    case 0b110:     exec_ori(insn, pos); return;
                    case 0b111:     exec_andi(insn, pos); return;
                    case 0b001:     exec_slli(insn, pos); return;
                    case 0b101:
                            switch(funct7)
                            {
                                default:            exec_illegal_insn(insn, pos); return;
                                case 0b0000000:     exec_srli(insn, pos); return;
                                case 0b0100000:     exec_srai(insn, pos); return;
                            }
                }
        //load
        case 0b0000011:
                switch(funct3)
                {
                    default:        exec_illegal_insn(insn, pos); return;
                    case 0b000:     exec_lb(insn, pos); return;
                    case 0b001:     exec_lh(insn, pos); return;
                    case 0b010:     exec_lw(insn, pos); return;
                    case 0b100:     exec_lbu(insn, pos); return;
                    case 0b101:     exec_lhu(insn, pos); return;
                }
        //B Type
        case 0b1100011:
                switch(funct3)
                {
                    default:        exec_illegal_insn(insn, pos); return;
                    case 0b000:     exec_beq(insn, pos); return;
                    case 0b001:     exec_bne(insn, pos); return;
                    case 0b100:     exec_blt(insn, pos); return;
                    case 0b101:     exec_bge(insn, pos); return;
                    case 0b110:     exec_bltu(insn, pos); return;
                    case 0b111:     exec_bgeu(insn, pos); return;
                }
        //J Type
        case 0b1101111:     exec_jal(insn, pos); return;
        case 0b1100111:     exec_jalr(insn, pos); return;

        //fence
        case 0b0001111:     exec_fence(insn, pos); return;
        //ecall and ebreak
        case 0b1110011:
                switch(suc)
                {
                    default:        exec_illegal_insn(insn, pos); return;
                //    case 0b000000000000:    exec_ecall(insn, pos); return;
                    case 0b000000000001:    exec_ebreak(insn, pos); return;
                }
        
    } 
}

/**
 *   Set the halt flag and, if the ostream* parameter is not nullptr
 *   use render_illegal_insn() to render the proper error message.
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
    halt = true;
    if(pos != nullptr)
    {
        std::string s = render_illegal_insn();
        s.resize(instruction_width, ' ');
        *pos << s;   
    }
}

/**
 *   ebreak execution method.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_ebreak(uint32_t insn, std::ostream* pos)
{
    if(pos)
    {
        std::string s = render_ebreak();
        s.resize(instruction_width, ' ');
        *pos << s << " // HALT" << std::endl;
    }
    halt = true;
}

/**
 *   lui execution method
 *   Set register rd to the imm_u value
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_lui(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    if(pos)
    {
        std::string s = render_lui(insn);
        s.resize(instruction_width, ' ');
        *pos << s << " // " << "x" << std::dec << rd << " = 0x" << hex32(imm_u) << std::endl;
    }
    regs.set(rd, imm_u);
    pc += 4;   
}

/**
 *   auipc execution method
 *   Add the address of the instruction to the imm_u value and store the result in register rd.
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_auipc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    int32_t val = imm_u + pc;

    if(pos)
    {
        std::string s = render_auipc(insn);
        s.resize(instruction_width, ' ');
        *pos << s << " // " << "x" << std::dec << rd << " = 0x" << hex32(pc) << " + " << "0x" << hex32(imm_u) << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   jal execution method
 *   Set register rd to the address of the next instruction that would otherwise be executed
 *   and then jump to the address given by the sum of the pc register and the imm_j value.
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_jal(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);
    
    int32_t val = pc + imm_j;

    if(pos)
    {
        std::string s = render_jal(insn);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(pc + 4) << ", pc = 0x" << hex32(pc) << " + 0x" << hex32(imm_j) << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd,pc+4);
    pc = val;
}

/**
 *   jalr execution method
 *   Set register rd to the address of the next instruction that would otherwise be executed
 *   and then jump to the address given by the sum of the rs1 register and the imm_i value
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_jalr(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    uint32_t rs1 = get_rs1(insn);

    int32_t val = (regs.get(rs1) + imm_i) & 0xfffffffe;
    
    if(pos)
    {
        std::string s = render_jalr(insn);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(pc + 4) << ", pc = (0x" << hex32(imm_i) << " + 0x" << hex32(regs.get(rs1)) << ") & 0xfffffffe = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, pc+4);
    pc = val;
}

/**
 *   bne execution method
 *   If rs1 is not equal to rs2 then add imm_b to the pc register.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_bne(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t t_addr;
    if(regs.get(rs1) != regs.get(rs2))
    {
      t_addr = pc + imm_b;
    }
    else
    {
        t_addr = pc + 4;
    }
    

    if(pos)
    {
        std::string s = render_btype(insn, "bne");
        s.resize(instruction_width, ' '); 
        *pos << s << " // pc += ("  << "0x" << hex32(regs.get(rs1)) << " != 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm_b) << " : 4) = 0x" << hex32(t_addr) << std::endl;
    }

    pc = t_addr; 
}

/**
 *   blt execution method
 *   If the signed value in rs1 is less than the signed value in rs2 then add imm_b to the pc register.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_blt(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t t_addr;
    if(regs.get(rs1) < regs.get(rs2))
    {
      t_addr = pc + imm_b;
    }
    else
    {
        t_addr = pc + 4;
    }
    

    if(pos)
    {
        std::string s = render_btype(insn, "blt");
        s.resize(instruction_width, ' '); 
        *pos << s << " // pc += (" << "0x" << hex32(regs.get(rs1)) << " < 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm_b) << " : 4) = 0x" << hex32(t_addr) << std::endl;
    }

    pc = t_addr;  
}

/**
 *   bge execution method
 *   If the signed value in rs1 is greater than or equal to the signed value in rs2 then add imm_b to the pc register.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_bge(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t t_addr;
    if(regs.get(rs1) >= regs.get(rs2))
    {
      t_addr = pc + imm_b;
    }
    else
    {
        t_addr = pc + 4;
    }
    

    if(pos)
    {
        std::string s = render_btype(insn, "bge");
        s.resize(instruction_width, ' '); 
        *pos << s << " // pc += (" << "0x" << hex32(regs.get(rs1)) << " >= 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm_b) << " : 4) = 0x" << hex32(t_addr) << std::endl;
    }
   pc = t_addr;
}

/**
 *   bltu execution method
 *   If the unsigned value in rs1 is less than the unsigned value in rs2 then add imm_b to the pc register.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_bltu(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t t_addr;
    if((uint32_t)regs.get(rs1) < (uint32_t)regs.get(rs2))
    {
        t_addr = pc + imm_b;
    }
    else
    {
        t_addr = pc + 4;
    }
    

    if(pos)
    {
        std::string s = render_btype(insn, "bltu");
        s.resize(instruction_width, ' '); 
        *pos << s << " // pc += (" << "0x" << hex32(regs.get(rs1)) << " <U 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm_b) << " : 4) = 0x" << hex32(t_addr) << std::endl;
    }
    
   pc = t_addr;
}

/**
 *   bgeu execution method
 *   If the unsigned value in rs1 is greater than or equal to the unsigned value in rs2 then add imm_b to the pc register.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t t_addr;
    if((uint32_t)regs.get(rs1) >= (uint32_t)regs.get(rs2))
    {
      t_addr = pc + imm_b;
    }
    else
    {
        t_addr = pc + 4;
    } 

    if(pos)
    {
        std::string s = render_btype(insn, "bgeu");
        s.resize(instruction_width, ' '); 
        *pos << s << " // pc += (" << "0x" << hex32(regs.get(rs1)) << " >=U 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm_b) << " : 4) = 0x" << hex32(t_addr) << std::endl;
    }
    
   pc = t_addr;
}

/**
 *   beq execution method
 *   If rs1 is equal to rs2 then add imm_b to the pc register.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_beq(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t t_addr;
    if((uint32_t)regs.get(rs1) == (uint32_t)regs.get(rs2))
    {
      t_addr = pc + imm_b;
    }
    else
    {
       t_addr = pc + 4;
    }
    

    if(pos)
    {
        std::string s = render_btype(insn, "beq");
        s.resize(instruction_width, ' '); 
        *pos << s << " // pc += (" << std::hex << "0x" << hex32(regs.get(rs1)) << " == 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm_b) << " : 4) = 0x" << hex32(t_addr) << std::endl;
    }
    
    pc = t_addr;
}

/**
 *   addi execution method
 *   Set register rd to rs1 + imm_i.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_addi(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t sum = regs.get(rs1) + imm_i;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "addi", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_i) << " = 0x" << hex32(sum) << std::endl; 
    }
    regs.set(rd, sum);
    pc += 4;
}

/**
 *   lbu execution method
 *   Set register rd to the value of the zero-extended byte fetched from the memory address given by the sum of rs1 and imm_i
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_lbu(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t t_addr = regs.get(rs1) + imm_i;
    uint32_t val = mem->get8(t_addr) & 0x000000ff;

    if(pos)
    {
        std::string s = render_itype_load(insn, "lbu");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = zx(m8(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_i) << ")) = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   lhu execution method
 *   Set register rd to the value of the zero-extended 16-bit little-endian half-word value fetched from
 *   the memory address given by the sum of rs1 and imm_i.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_lhu(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t t_addr = regs.get(rs1) + imm_i;
    uint32_t val = mem->get16(t_addr) & 0x0000ffff;

    if(pos)
    {
        std::string s = render_itype_load(insn, "lhu");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = zx(m16(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_i) << ")) = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   lb execution method
 *   Set register rd to the value of the sign-extended byte fetched from the memory address 
 *   given by the sum of rs1 and imm_i
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_lb(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t t_addr = regs.get(rs1) + imm_i;
    uint32_t val = mem->get8(t_addr);

    val = 0xff & (int32_t)val;
    int32_t num = 0x80;
    if(num & val)
    {
        val += 0xffffff00;
    }

    if(pos)
    {
        std::string s = render_itype_load(insn, "lb");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = sx(m8(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_i) << ")) = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   lh execution method
 *   Set register rd to the value of the sign-extended 16-bit little-endian half-word value fetched from 
 *   the memory address given by the sum of rs1 and imm_i.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_lh(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t t_addr = regs.get(rs1) + imm_i;
    uint32_t val = mem->get16(t_addr);

    val = 0xffff &(int32_t)val;
    int32_t num = 0x8000;
    if(num & val)
    {
        val += 0xffff0000;
    }

    if(pos)
    {
        std::string s = render_itype_load(insn, "lh");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = sx(m16(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_i) << ")) = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   lw execution method
 *   Set register rd to the value of the sign-extended 32-bit little-endian word value fetched from the
 *   memory address given by the sum of rs1 and imm_i.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_lw(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t t_addr = regs.get(rs1) + imm_i;
    uint32_t val = mem->get32(t_addr);

    if(pos)
    {
        std::string s = render_itype_load(insn, "lw");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = sx(m32(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_i) << ")) = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   sb execution method
 *   Set the byte of memory at the address given by the sum of rs1 and imm_s to the 8 LSBs of rs2.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sb(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn); 

    uint32_t t_addr = regs.get(rs1) + imm_s;
    uint32_t val = regs.get(rs2) & 0x000000ff;

    if(pos)
    {
        std::string s = render_stype(insn, "sb");
        s.resize(instruction_width, ' ');
        *pos << s << " // m8(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_s) << ") = 0x" << hex32(val) << std::endl;
    }
    mem->set8(t_addr, val);
    pc += 4;
}

/**
 *   sh execution method
 *   Set the 16-bit half-word of memory at the address given by the sum of rs1 and imm_s to the 16 LSBs of rs2.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sh(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn); 

    uint32_t t_addr = regs.get(rs1) + imm_s;
    uint32_t val = regs.get(rs2) & 0x0000ffff;

    if(pos)
    {
        std::string s = render_stype(insn, "sh");
        s.resize(instruction_width, ' ');
        *pos << s << " // m16(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_s) << ") = 0x" << hex32(val) << std::endl;
    }
    mem->set16(t_addr, val);
    pc += 4;
}

/**
 *   sw execution method
 *   Store the 32-bit value in rs2 into the memory at the address given by the sum of rs1 and imm_s.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sw(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn); 

    uint32_t t_addr = regs.get(rs1) + imm_s;
    uint32_t val = regs.get(rs2);

    if(pos)
    {
        std::string s = render_stype(insn, "sw");
        s.resize(instruction_width, ' ');
        *pos << s << " // m32(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm_s) << ") = 0x" << hex32(val) << std::endl;
    }
    mem->set32(t_addr, val);
    pc += 4;
}

/**
 *   slti execution method
 *   If the signed integer value in rs1 is less than the signed integer value in imm_i then set rd to 1.
 *   Otherwise, set rd to 0.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_slti(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = (regs.get(rs1) < imm_i) ? 1 : 0; 

    if(pos)
    {
        std::string s = render_itype_alu(insn, "slti", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " < " << std::dec << imm_i << ") ? 1 : 0 = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd,val);
    pc += 4;
    
}

/**
 *   sltiu execution method
 *   If the unsigned integer value in rs1 is less than the unsigned integer value in imm_i then set rd to 1.
 *   Otherwise, set rd to 0.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = ((uint32_t)regs.get(rs1) < (uint32_t)imm_i) ? 1 : 0;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "sltiu", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " <U " << std::dec << imm_i << ") ? 1 : 0 = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
    
}

/**
 *   xori execution method
 *   Set register rd to the bitwise xor of rs1 and imm_i.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_xori(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = regs.get(rs1) ^ imm_i;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "xori", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " ^ 0x" << hex32(imm_i) << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   ori execution method
 *   Set register rd to the bitwise or of rs1 and imm_i.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_ori(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = regs.get(rs1) | imm_i;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "ori", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " | 0x" << hex32(imm_i) << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   andi execution method
 *   Set register rd to the bitwise and of rs1 and imm_i.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_andi(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    int32_t val = (regs.get(rs1) & imm_i);

    if(pos)
    {
        std::string s = render_itype_alu(insn, "andi", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " & 0x" << hex32(imm_i) << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   slli execution method
 *   Shift rs1 left by the number of bits given in shamt_i and store the result in rd.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_slli(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t shift = get_imm_i(insn)%XLEN;

    int32_t val = regs.get(rs1) << shift;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "slli", get_imm_i(insn));
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " << " << std::dec << shift << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   srli execution method
 *   Logic-shift rs1 right by the number of bits given in shamt_i and store the result in rd.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_srli(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t shift = get_imm_i(insn)%XLEN;;

    int32_t val = (uint32_t)regs.get(rs1) >> shift;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "srli", get_imm_i(insn));
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shift << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   srai execution method
 *   Arithmetic-shift rs1 right by the number of bits given in shamt_i and store the result in rd.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_srai(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t shift = get_imm_i(insn)%XLEN;

    int32_t val = regs.get(rs1) >> shift;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "srai", (int32_t)get_imm_i(insn)%XLEN);
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shift << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   add execution method
 *   Set register rd to rs1 + rs2.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_add(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    int32_t val = regs.get(rs1) + regs.get(rs2);

    if(pos)
    {
        std::string s = render_rtype(insn, "add");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4; 
}

/**
 *   sub execution method
 *   Set register rd to rs1 - rs2.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sub(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    int32_t val = regs.get(rs1) - regs.get(rs2);

    if(pos)
    {
        std::string s = render_rtype(insn, "sub");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " - 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(val) << std::endl;
    }
    regs.set(rd, val);
    pc += 4; 
}

/**
 *   sll execution method
 *   Shift rs1 left by the number of bits specified in the least significant five bits of rs2 and store the result in rd.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sll(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    int32_t shamt = regs.get(rs2) & 0x01f;
    int32_t val = regs.get(rs1) << shamt;

    if(pos)
    {
        std::string s = render_rtype(insn, "sll");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " << " << std::dec << shamt << " = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   slt execution method
 *   If the signed integer value in rs1 is less than the signed integer value in rs2 then set rd to 1.
 *   Otherwise, set rd to 0.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_slt(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    
    int32_t val = (regs.get(rs1) < regs.get(rs2)) ? 1 : 0; 

    if(pos)
    {
        std::string s = render_rtype(insn, "slt");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " < 0x" << hex32(regs.get(rs2)) << ") ? 1 : 0 = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   sltu execution method
 *   If the unsigned integer value in rs1 is less than the unsigned integer value in rs2 then set rd to 1.
 *   Otherwise, set rd to 0.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sltu(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    
    int32_t val = ((uint32_t)regs.get(rs1) < (uint32_t)regs.get(rs2)) ? 1 : 0; 

    if(pos)
    {
        std::string s = render_rtype(insn, "sltu");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " <U 0x" << hex32(regs.get(rs2)) << ") ? 1 : 0 = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   xor execution method
 *   Set register rd to the bitwise xor of rs1 and rs2.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_xor(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    
    int32_t val = regs.get(rs1) ^ regs.get(rs2); 

    if(pos)
    {
        std::string s = render_rtype(insn, "xor");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " ^ 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   srl execution method
 *   Logic-shift rs1 right by the number of bits given in rs2 and store the result in rd.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_srl(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    int32_t shift = regs.get(rs2) & 0x0000001f;
    int32_t val = (uint32_t)regs.get(rs1) >> shift; 

    if(pos)
    {
        std::string s = render_rtype(insn, "srl");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shift << " = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   sra execution method
 *   Arithmetic-shift rs1 right by the number of bits given in rs2 and store the result in rd.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_sra(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    int32_t shift = regs.get(rs2) & 0x0000001f;
    int32_t val = regs.get(rs1) >> shift; 

    if(pos)
    {
        std::string s = render_rtype(insn, "sra");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" <<  std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shift << " = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   and execution method
 *   Set register rd to the bitwise and of rs1 and rs2.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_and(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    
    int32_t val = regs.get(rs1) & regs.get(rs2); 

    if(pos)
    {
        std::string s = render_rtype(insn, "and");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " & 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   or execution method
 *   Set register rd to the bitwise or of rs1 and rs2.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_or(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);

    
    int32_t val = regs.get(rs1) | regs.get(rs2); 

    if(pos)
    {
        std::string s = render_rtype(insn, "or");
        s.resize(instruction_width, ' ');
        *pos << s << " // x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " | 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(val) << std::endl; 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 *   fence execution method.
 *   
 *
 * 
 * @param insn
 * @param pos
 * @return void
 *   
 ********************************************************************************/
void rv32i::exec_fence(uint32_t insn, std::ostream* pos)
{
    if(pos)
    {
        std::string s = render_fence(insn);
        s.resize(instruction_width, ' ');
        *pos << s << " // fence" << std::endl;
    }
    pc +=4;
}