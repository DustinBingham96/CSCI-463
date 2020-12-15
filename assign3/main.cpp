//******************************************************************
//
// CSCI 463 Assignment 3
//
// Author: Dustin Bingham
//
// Memory Simulator
//
//******************************************************************
#include<iostream>
#include "memory.h"
#include "hex.h"

/**
 * When called will print a proper usage message to the user and exit the program.
 *
 * 
 * 
 * 
 *
 * @param  n/a.
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
void usage()
{
    std::cerr << "Usage: ./memory fileSize fileName";
    exit(0);
}


/**
 * Main function of the memory program used to print contents stored in memory.
 *
 * 
 * 
 * 
 *
 * @param  n/a.
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
int main(int argc, char **argv)
{
    if (argc != 3)
        usage();
    
    memory mem(std::stoul(argv[1], 0, 16));
    mem.dump();

    if(!mem.load_file(argv[2]))
        usage();
    mem.dump();

    std::cout << mem.get_size() << std::endl;
    std::cout << hex32(mem.get8(0)) << std::endl;
    std::cout << hex32(mem.get16(0)) << std::endl;
    std::cout << hex32(mem.get32(0)) << std::endl;
    std::cout << hex0x32(mem.get8(0)) << std::endl;
    std::cout << hex0x32(mem.get16(0)) << std::endl;
    std::cout << hex0x32(mem.get32(0)) << std::endl;
    std::cout << hex8(mem.get8(0)) << std::endl;
    std::cout << hex8(mem.get16(0)) << std::endl;
    std::cout << hex8(mem.get32(0)) << std::endl;
    std::cout << hex0x32(mem.get32(0x1000)) << std::endl;

    mem.set8(0x10, 0x12);
    mem.set16(0x14, 0x1234);
    mem.set32(0x18, 0x87654321);
    mem.dump();

    return 0;
}
