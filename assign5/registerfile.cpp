//******************************************************************
//
// CSCI 463 Assignment 5
//
// Author: Dustin Bingham
//
// RISC-V Simulator
//
//******************************************************************
#include "registerfile.h"
#include "rv32i.h"
#include<cstdint>
#include<iostream>
#include<iomanip>

/**
 *   constructor for registerfile.
 *   
 *
 * 
 *   
 ********************************************************************************/
registerfile::registerfile()
{
    reset();
}

/**
 *   
 *   Initialize register x0 to zero, and all other registers to 0xf0f0f0f0.
 *
 *   @return void.
 *   
 ********************************************************************************/
void registerfile::reset(){
    reg[0]= 0x0;
    for(int i=1; i<32; i++)
    {
        reg[i] = 0xf0f0f0f0;
    }
}

/**
 *   
 *   Assign register r the given val. If r is zero, then do nothing.
 *
 *   @param r
 *   @param val
 *   @return void
 *   
 ********************************************************************************/
void registerfile::set(uint32_t r, int32_t val)
{
    if(r==0)
    {
       //do nothing 
    }
    else //set register r to val
    {
        reg[r]=val; 
    }
    
}

/**
 *   
 *   Return the value of register r. If r is zero, then return zero.
 *
 *   @param r
 *   @return int32_t
 *   
 ********************************************************************************/
int32_t registerfile::get(uint32_t r) const
{
    if(r==0)
    {
        return 0;
    }
    else
    {
        return reg[r];
    }
}

/**
 *   
 *   dumps in the following format:
 *   
 *   x0 00000000 f0f0f0f0 00001000 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0
 *   x8 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0   
 *  x16 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0
 *  x24 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0
 * 
 * 
 *   @return void
 *   
 ********************************************************************************/
void registerfile::dump() const
{
    //dump contents of registers
    for(size_t i = 0; i < 32; i++) 
    {
        //print newline after 8th entry if first line
        if(i != 0 && i % 8 == 0)
        {
            std::cout << std::endl;
        }
        //headers
        if(i % 8 == 0)
        {
            std::string h;
            if(i==0)
            { 
                h = "x0";
            }
            else if(i == 8)
            {
                h = "x8";
            }
            else if(i == 16)
            {
                h = "x16";
            }
            else if(i == 24)
            {
                h = "x24";
            }
            
            
            std::cout << std::dec << std::right << std::setw(3) << std::setfill(' ') << h << " ";
        }
        //print contents of the register
        if(i % 8 == 7)
        {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << reg[i];
        }
        else
        {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << reg[i] << " ";
        }
        
    }
    std::cout << std::endl;
}