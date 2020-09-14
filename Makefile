# Compilator
CPP = g++

# Options of compilation
CFLAGS = -g -Wall  -lglpk -ligraph -lm
LFLAGS =

## Build target executable
#TARGET = main

# Additionals
OBJS	= main.o block.o clustering.o utils.o solution.o inputdata.o
SOURCE	= main.cc block.cc clustering.cc utils.cc solution.cc inputdata.cc
HEADER	= ./heads/block.h ./heads/clustering.h ./heads/utils.h ./heads/solution.h ./heads/inputdata.h
OUT	= a.out


all: $(OBJS).cc
	$(CPP) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cc
	$(CPP) $(CFLAGS) main.cc -std=c11

block.o: block.cc
	$(CPP) $(CFLAGS) block.cc -std=c11

clustering.o: clustering.cc
	$(CPP) $(CFLAGS) clustering.cc -std=c11

utils.o: utils.cc
	$(CPP) $(CFLAGS) utils.cc -std=c11

solution.o: solution.cc
	$(CPP) $(CFLAGS) solution.cc -std=c11

inputdata.o: inputdata.cc
	$(CPP) $(CFLAGS) inputdata.cc -std=c11

clean:
	rm -f $(OBJS) $(OUT)