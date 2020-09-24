
# Compilator
CPP = g++

# Options of compilation
DBG=-g
CFLAGS = -Wall

# Additionnal libraries inclusion
LDLIBS = -lglpk -ligraph -lm

## Build target executable
#TARGET = main

# Directories
HEADS 	= ./heads
#OBJS	= ./main.o ./block.o ./clustering.o ./solution.o ./utils.o ./inputdata.o
OBJS	= ./objs
SOURCE	= main.cc block.cc clustering.cc solution.cc utils.cc inputdata.cc
HEADER	= $(HEADS)/block.h $(HEADS)/clustering.h $(HEADS)/solution.h $(HEADS)/utils.h $(HEADS)/inputdata.h
OUT	= a.out


all: $(OBJS)/main.o $(OBJS)/clustering.o $(OBJS)/solution.o $(OBJS)/utils.o $(OBJS)/inputdata.o
	$(CPP) $(DBG) $(LDLIBS) $(CFLAGS) -o $(OUT) $^

$(OBJS)/main.o: main.cc
	$(CPP) $(DBG) -o $@ -c $< $(CFLAGS)

#block.o: block.cc
#	$(CPP) -c $(CFLAGS) block.cc -std=c++11

$(OBJS)/clustering.o: clustering.cc
	$(CPP) $(DBG) -o $@ -c $< $(CFLAGS)

$(OBJS)/solution.o: solution.cc
	$(CPP) $(DBG) -o $@ -c $< $(CFLAGS)

$(OBJS)/utils.o: utils.cc
	$(CPP) $(DBG) -o $@ -c $< $(CFLAGS)

$(OBJS)/inputdata.o: inputdata.cc
	$(CPP) $(DBG) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(OBJS)/*.o