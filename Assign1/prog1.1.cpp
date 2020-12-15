//******************************************************************
//
// CSCI 463 Assignment 1
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

//using namespace std;

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
 * @note This is how you can add an optional note about the function that
 *    may be of interest to someone using it.
 *
 * @warning This is how you can add an optional warning to a user of the
 *    function suitable for noting things like 'This function is not thread
 *    safe' and so on.
 *
 * @bug This is how you can add an optional description of a known bug in the
 *    function such as: This only works for positive values of z.
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

	for (int i = 31; i >= 0; i--)
	{
		if(i == 27 || i == 23 || i == 19 || i == 15 || i == 11 || i == 7 || i == 3 )
		{
			cout << " ";
		}
		cout << ((x >> i) & 1); 
	}

	cout << endl;

	//Sign

	cout << "sign: " << sign <<endl;

	//Exp

	cout << " exp: 0x" << setfill('0') << setw(8) << right << hex << exp;
	cout << " (" << dec << exp << ")" << endl;

	//Sig 

	cout << " sig: 0x" << setfill('0') << setw(8) << right << hex << sig << endl;

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

	//exp is positive

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
	
	//exp is negative

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
 * Prompts user for IEEE hex number.
 *
 * Loops on cin until there is no more data to process
 * 
 * 
 *
 * @param n/a.
 *
 * @return n/a.
 *
 * @note This is how you can add an optional note about the function that
 *    may be of interest to someone using it.
 *
 * @warning This is how you can add an optional warning to a user of the
 *    function suitable for noting things like 'This function is not thread
 *    safe' and so on.
 *
 * @bug This is how you can add an optional description of a known bug in the
 *    function such as: This only works for positive values of z.
 ********************************************************************************/

int main()
{
	uint32_t val;
	//cout << "please enter hex IEEE number's: ";
	while (cin >> hex >> val)
	{
		printBinFloat(val);
	}

	return 0;
}
