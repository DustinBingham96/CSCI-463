#ifndef RV32I_H
#define RV32I_H
#include<string>
#include<cstdint>
#include "memory.h"

static constexpr int mnemonic_width = 8;
static constexpr int instruction_width = 35;

class rv32i
{
    public:
        
        rv32i(memory *m);
        void disasm(void);
        std::string decode(uint32_t insn) const;
        static uint32_t get_opcode(uint32_t insn);
        static uint32_t get_rd(uint32_t insn);
        static uint32_t get_funct3(uint32_t insn);
        static uint32_t get_rs1(uint32_t insn);
        static uint32_t get_rs2(uint32_t insn);
        static uint32_t get_funct7(uint32_t insn);
        static int32_t get_imm_i(uint32_t insn);
        static int32_t get_imm_u(uint32_t insn);
        static int32_t get_imm_b(uint32_t insn);
        static int32_t get_imm_s(uint32_t insn);
        static int32_t get_imm_j(uint32_t insn);
        //helper
        std::string render_illegal_insn() const;
        std::string render_lui(uint32_t insn) const;
        std::string render_auipc(uint32_t insn) const;
        std::string render_jal(uint32_t insn) const;
        std::string render_jalr(uint32_t insn) const;
        std::string render_btype(uint32_t insn, const char *mnemonic) const;
        std::string render_itype_load(uint32_t insn, const char *mnemonic) const;
        std::string render_stype(uint32_t insn, const char *mnemonic) const;
        std::string render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i) const;
        std::string render_rtype(uint32_t insn, const char *mnemonic) const;
        std::string render_fence(uint32_t insn) const;
        std::string render_ecall() const;
        std::string render_ebreak() const;








    private:
        memory * mem;
        uint32_t pc;
        static constexpr uint32_t XLEN = 32;


};

#endif
