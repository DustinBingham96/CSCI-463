#
# PROGRAM: Assignment 1
# Author:  Dustin Bingham
#

CXXFLAGS = -Wall -Werror -std=c++11

prog1: prog1.o
	g++ $(CXXFLAGS) -o prog1 prog1.o
prog1.o: prog1.cpp
	g++ $(CXXFLAGS) -c prog1.cpp
clean:
	rm -f *.o prog1
