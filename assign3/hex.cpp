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
#include<iomanip>
#include <sstream>
#include "hex.h"


/**
 *  returns hex string with exactly 2 hex digits representing the 
 *  8 bits of the i argument     
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
std::string hex8(uint8_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
    return os.str();
}

/**
 *  returns hex string with exactly 8 hex digits representing the 
 *  32 bits of the i argument     
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
std::string hex32(uint32_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(8) << uint32_t(i);
    return os.str();
}

/**
 *  returns hex string beginning with '0x' followed by the
 *  8 hex digits representing the 32 bits of the i argument     
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
std::string hex0x32(uint32_t i)
{
    return std::string("0x")+hex32(i);
}