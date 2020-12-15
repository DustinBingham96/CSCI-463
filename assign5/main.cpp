//******************************************************************
//
// CSCI 463 Assignment 5
//
// Author: Dustin Bingham
//
// RISC-V Simulator
//
//******************************************************************
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "memory.h"
#include "rv32i.h"
using namespace std;
/**
 * Print a usage message and abort the program.
 * ********************************************************************/
static void usage()
{
    cerr << "Usage: rv32i [-m hex-mem-size] infile" << endl;
    cerr << "   -m specify memory size (default = 0x10000)" << endl;
    exit(1);
}


/**
 * Read a file of RV32I instructions and execute them.
********************************************************************/
int main(int argc, char **argv)
{
    uint32_t memory_limit = 0x10000; //default memory size = 64k
    int opt;
    int execution_limit = 0;
    int dflag = 0;
    int iflag = 0;
    int rflag = 0;
    int zflag = 0;

    while((opt = getopt(argc, argv, "m:l:dirz")) != -1)
    {
        switch(opt)
        {
            case 'm':
                memory_limit = stoul(optarg, nullptr, 16);
                break;
            
            case 'd':
                dflag = 1;
                //std::cout << "d" << std::endl;
                break;
            
            case 'i':
                iflag = 1;
                //std::cout << "i" << std::endl;
                break;
            
            case 'r':
                rflag = 1;
                //std::cout << "r" << std::endl;
                break;
            
            case 'z':
                zflag = 1;
                //std::cout << "z" << std::endl;
                break;

            case '1':
                execution_limit = std::stoul(optarg, nullptr, 0);
                break;
            
            default: /* '?' */
                usage();
        }
    }

    if(optind >= argc)
        usage();
    
    memory mem(memory_limit);

    if(!mem.load_file(argv[optind]))
        usage();

    
    rv32i sim(&mem);

    if(dflag == 1) 
    {
        sim.disasm();
        sim.reset();
    }

    if(iflag == 1)
    {
        sim.set_show_instructions(true);
    }
    if(rflag == 1)
    {
        sim.set_show_registers(true);
    }
    sim.run(execution_limit);
    if(zflag == 1) 
    {
        sim.dump();
        mem.dump();
    }
    
    

    return 0;
}