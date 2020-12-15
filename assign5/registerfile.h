//******************************************************************
//
// CSCI 463 Assignment 5
//
// Author: Dustin Bingham
//
// RISC-V Simulator
//
//******************************************************************
#ifndef reg_H
#define reg_H
#include <stdint.h>
class registerfile
{
    public:
        registerfile();
        void reset();
        void set(uint32_t r, int32_t val);
        int32_t get(uint32_t r) const;
        void dump() const;


    private:
        int32_t reg[32];
};
#endif