#
# PROGRAM: Assignment 3
# Author:  Dustin Bingham
#
CCFLAGS = -ansi -pedantic -Wall -Werror -Wextra -g -std=c++11
memory: main.o memory.o hex.o
    g++ $(CCFLAGS) -o memory main.o memory.o hex.o
main.o: main.cpp hex.h memory.h
    g++ $(CCFLAGS) -c main.cpp
hex.o: hex.cpp hex.h
    g++ $(CCFLAGS) -c hex.cpp
memory.o: memory.cpp memory.h
    g++ $(CCFLAGS) -c memory.cpp
clean:
    -rm *.o main