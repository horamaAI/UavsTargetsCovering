# Compilator
CPP = g++

# Options of compilation
CFLAGS = -g -Wall  -lglpk -ligraph -lm
LFLAGS =

## Build target executable
#TARGET = main

# Additionals
SRCS	= ./srcs
HEADS 	= $(SRCS)/heads
OBJS	= $(SRCS)/main.o $(SRCS)/block.o $(SRCS)/clustering.o $(SRCS)/utils.o $(SRCS)/solution.o $(SRCS)/inputdata.o
SOURCE	= main.cc block.cc clustering.cc utils.cc solution.cc inputdata.cc
HEADER	= $(HEADS)/block.h $(HEADS)/clustering.h $(HEADS)/utils.h $(HEADS)/solution.h $(HEADS)/inputdata.h
OUT	= a.out


all: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT) $(LFLAGS)

$(SRCS)/main.o: $(SRCS)/main.cc
	$(CPP) $(CFLAGS) $(SRCS)/main.cc -std=c11

$(SRCS)/block.o: $(SRCS)/block.cc
	$(CPP) $(CFLAGS) $(SRCS)/block.cc -std=c11

$(SRCS)/clustering.o: $(SRCS)/clustering.cc
	$(CPP) $(CFLAGS) $(SRCS)/clustering.cc -std=c11

$(SRCS)/utils.o: $(SRCS)/utils.cc
	$(CPP) $(CFLAGS) $(SRCS)/utils.cc -std=c11

$(SRCS)/solution.o: $(SRCS)/solution.cc
	$(CPP) $(CFLAGS) $(SRCS)/solution.cc -std=c11

$(SRCS)/inputdata.o: $(SRCS)/inputdata.cc
	$(CPP) $(CFLAGS) $(SRCS)/inputdata.cc -std=c11

clean:
	rm -f $(OBJS) $(OUT)