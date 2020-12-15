//******************************************************************
//
// CSCI 463 Assignment 3
//
// Author: Dustin Bingham
//
// Memory Simulator
//
//******************************************************************
#include <iostream>
#include <string>
#include <stdint.h>
#include <fstream>
#include<iomanip>
#include "memory.h"
#include "hex.h"

/**
 *  Constructor for the memory array
 *  Auto sets every address in memory to 0xa5    
 *
 * @param  i.
 *
 * @return std::string.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
memory::memory(uint32_t siz)
{
    siz = (siz+15)&0xfffffff0;
    size = siz;
    mem = new uint8_t[size];
    

    for(unsigned int i = 0; i<size; i++)
    {
        mem[i] = 0xa5;
    }
    
}

/**
 *  Destructor for the memory array
 *  Deletes the array for actual memory    
 *
 * 
 *
 * @return n/a.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
memory::~memory()
{
    delete [] mem;
    //size = NULL;
}


/**
 *  returns true if the given address is in your stimulated memory.
 *  prints warning message otherwise
 *
 * @param  i.
 *
 * @return bool.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
bool memory::check_address(uint32_t i) const
{
    if (i <= size) 
    {
        return true;
    }

    std::cout<<"WARNING: Address out of range: " << hex0x32(i) << std::endl;
    return false;

}


/**
 *  Return the (possibly rounded up) siz value
 *      
 *
 * @param  n/a.
 *
 * @return uint32_t.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
uint32_t memory::get_size() const
{
    return size;
}


/**
 *  Check to see if the given addr is in your mem by calling
 *  check_address(). If addr is in the valid range, return the value    
 *  of the byte from your simulated memory at that address. If addr
 *  is not in the valid range then return zero to the caller
 * 
 * @param  addr
 *
 * @return uint8_t.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
uint8_t memory::get8(uint32_t addr) const
{
    if(check_address(addr) == true)
    {
        return mem[addr];
    }

    return 0;
}

/**
 *  Must call get8() twice to get two bytes and then combine them in
 *  little-endian order to create a 16-bit return value.    
 *
 * @param  addr
 *
 * @return uint32_t.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
uint16_t memory::get16(uint32_t addr) const
{
    uint8_t temp1 = get8(addr); //get the first half
    uint8_t temp2 = get8(addr+1); //get the second half

    return(uint16_t)((temp2 << 8) | (temp1)); //return in little endian format
}


/**
 *  Must call get16() twice and combine results in litte-endian
 *  order to the implementation of get16()    
 *
 * @param  addr
 *
 * @return uint32_t.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
uint32_t memory::get32(uint32_t addr) const
{
    uint16_t temp1 = get16(addr); //get the first half
    uint16_t temp2 = get16(addr+2); //get the second half

    //little endian formatting
    return(uint32_t)((temp2 << 16) | (temp1));
}


/**
 *  Calls check_address to verify the addr argument is valid.
 *      
 *
 * @param  addr
 * @param  val
 *
 * @return void.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
void memory::set8(uint32_t addr, uint8_t val)
{
    if(check_address(addr))
    {
       mem[addr] = val;
    }


}


/**
 *  Calls set8() twice to store the given val in little-endian order
 *  into the simulated memory starting at the address given    
 *
 * @param  addr
 * @param  val
 *
 * @return void.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
void memory::set16(uint32_t addr, uint16_t val)
{
    set8(addr, val);
    set8(addr+1, val >> 8);

    
}


/**
 *  Calls set16() twice to store the given val in litte-endian order into
 *  the simulated memory starting at the address given    
 *
 * @param  addr
 * @param  val
 *
 * @return void.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
void memory::set32(uint32_t addr, uint32_t val)
{
    set16(addr, val);
    set16(addr+2, val >> 16);
}


/**
 *  Dump the entire contents of simulated memory in hex with ASCII on the
 *  right exactly, space for space in the format given    
 *
 *
 * @return void.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
void memory::dump() const
{
    
    char ascii[16]; //ascii array

    for(unsigned int i = 0; i < size; i++)
    {
        if(i !=0 && i%16 == 0)
        {
            ascii[16] = '\0';
            std::cout<< "*" << ascii << "*" << std::endl;
        }
        if(i%16 == 0)
        {
            std::cout << std::hex << std::setfill('0') << std::setw(8) << i << ": ";
        }
        else if(i%8 == 0)
        {
            std::cout << " ";
        }

        std::cout << std::hex << std::setw(2) << std::setfill('0') << (uint16_t)mem[i] << " ";
        uint8_t ch = get8(i);
        ascii[i%16] = isprint(ch) ? ch : '.';
    }

    if(size > 0)
    {
        std::cout << "*" << ascii << "*" << std::endl;
    }

    std::cout << std::dec; //when done return to decimal output.
    
}


/**
 *  Open the file named fname in binary mode and read its contents into
 *  simulated memory.    
 *
 * @param  string
 * @param  fname 
 *
 * @return void.
 *
 * @note n/a.
 *    
 *
 * @warning n/a.
 *    
 *    
 *
 * @bug n/a.
 *   
 ********************************************************************************/
bool memory::load_file(const std::string &fname)
{
    std::ifstream infile(fname, std::ios::in|std::ios::binary);
    if(!infile)
    {
        std::cerr<<"Can't open file '" << fname <<"' for reading." << std::endl;
    }
    int i = 0;


    while(infile.good())
    {
        if(check_address(i)) 
        {
            infile >> std::noskipws >> mem[i]; //read in from the file and do not skip whitespaces
        }
        else
        {
            std::cerr<< "Program too big." << std::endl;
            return false;
        }
        ++i;
        
    }

    infile.close(); //close the file
    return true;

}






