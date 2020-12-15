//******************************************************************
//
// CSCI 463 Assignment 2
//
// Author: Dustin Bingham
//
// Bitwise Operators & IEEE-754 Floating Point Number Decoding
//
//******************************************************************

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <stdlib.h>

using std::cin;
using std::cout;
using std::hex;
using std::dec;
using std::endl;
using std::setfill;
using std::setw;
using std::right;



/**
 * Read 0 or more 32bit hex values and print in binary.
 *
 * Print by extracting the sign, exponent and significand from the
 * 32-bit value. Must use bitwise operators to extract and shift
 * the fields of the IEEE number.
 *
 * @param x int32_t value to convert.
 *
 * @return n/a.
 *
 * @note Find a more efficient way to split binary output in the first line
 * 		 of printing.
 *
 * @warning n/a.
 *    
 *  
 *
 * @bug n/a.
 *    
 ********************************************************************************/

void  printBinFloat(int32_t x)
{
	const uint32_t a = 0x7f800000; //const hex value for extracting exp
	const uint32_t b = 0x007fffff; //const hex value for extracting sig
	const uint32_t c = 0x80000000; //const hex value for extracting sign
	const uint32_t inf = 0x7f800000; //const hex infinity
	const int32_t ninf = 0xff800000; //const hex negative infinity
	const int32_t nzero = 0x80000000; //const hex negative 0

	//extract and set exponent, sig and sign
	int32_t exp = ((x & a) >> 23) - 127;
	uint32_t sig = x & b;
	bool sign = x & c;
	

	//print initial information

	//First Line
	cout << "0x" << setfill('0') << setw(8) << hex << x << " = ";

	//Loop for binary conversion and printing
	for (int i = 31; i >= 0; i--)
	{
		//Splits binary output with a space after every 4 bits
		if(i == 27 || i == 23 || i == 19 || i == 15 || i == 11 || i == 7 || i == 3 )
		{
			cout << " ";
		}

		//method to convert to binary using bitwise operators
		cout << ((x >> i) & 1); 
	}

	cout << endl;

	//Sign portion of the output line

	cout << "sign: " << sign <<endl;

	//Exp portion of the output line

	cout << " exp: 0x" << setfill('0') << setw(8) << right << hex << exp;
	cout << " (" << dec << exp << ")" << endl;

	//Sig portion of the output line

	cout << " sig: 0x" << setfill('0') << setw(8) << right << hex << sig << endl;

	//Final line formatting and error checking

	if (sign == false)
	{
		cout << "+";
	}

	else
	{
		cout << "-";
	}

	//infinity cases

	if (x == inf || x == ninf)
	{
		cout << "inf" << endl;
		return;
	}

	//zero case

	if (x == 0)
	{
		cout << "0" << endl;
		return;
	}

	if (x == nzero)
	{
		cout << "0" << endl;
		return;
	}

	//when exp is positive

	if (exp >= 0)
	{
		int digits = exp + 23; //variable to keep track of digits

		cout << "1"; //1 always leads the significand
		sig = (sig << 9); //shift the bits in significand

		for (int i = 0; i < digits; i++)
		{
			if (i == (int)exp) //insert decimal point when i = exp
			{
				cout << ".";
			}

			cout << ((sig & 0x80000000) ? '1' : '0'); //print 1 if first bit is 1 otherwise 0

			sig = (sig << 1); //itterate significand

			if (i >= (int)exp && i > 23)//error checking
			{
				break;
			}

		}

	}
	
	//when exp is negative

	else
	{
		cout << "0."; //print a zero
		sig = (sig << 9); //shift the bits in significand
		int lzeros = abs((int)exp) - 1; //variable stores number of leading zeros before sig

		for (int i = 0; i < lzeros; i++) //print out leading significant zeros
		{
			cout << "0";
		}

		cout << "1"; //implied zero

		for (int i = 0; i < 23; i++)
		{
			cout << ((sig & 0x80000000) ? '1' : '0');
			sig = (sig << 1);
		}
		
	}

	cout << endl;

}

/**
 * Calls above function printBinFloat with some val given through cin.
 *
 * Loops on cin until there is no more data to process and does not prompt the user
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

int main()
{
	uint32_t val; //variable to be used by cin and passed into printBinFloat
	while (cin >> hex >> val)
	{
		printBinFloat(val);
	}

	return 0;
}
