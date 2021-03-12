
# Compilator
CPP		= g++
NAME 	= uas_deplo
# Build target executable
TRGT	= $(NAME)
# Options of compilation
DBG=-g
CXXFLAGS = -O0 -std=c++11 -Wall -Wextra -Wshadow -pedantic -Weffc++ -Werror -DDEBUG
# Additionnal libraries inclusion
LDLIBS = -lglpk -ligraph -lm
# Directories
HEADS 	= ./heads
#OBJS	= ./main.o ./block.o ./clustering.o ./glpk_linear_solver.o ./solution.o ./utils.o ./inputdata.o
OBJSDIR	= ./objs
SRCS	= main.cc block.cc clustering.cc glpk_linear_solver.cc solution.cc utils.cc inputdata.cc debug.cc
HEADER	= $(HEADS)/block.h $(HEADS)/clustering.h $(HEADS)/glpk_linear_solver.h $(HEADS)/solution.h\
	$(HEADS)/utils.h $(HEADS)/inputdata.h $(HEADS)/debug.h
OUT	= a.out

all: $(OBJS)/main.o $(OBJS)/clustering.o $(OBJS)/glpk_linear_solver.o $(OBJS)/solution.o $(OBJS)/utils.o\
	$(OBJS)/inputdata.o $(OBJS)/debug.o
	$(CPP) $(DBG) $(LDLIBS) $(CXXFLAGS) -o $(OUT) $^

$(OBJS)/main.o: main.cc
	$(CPP) $(DBG) -o $@ -c $< $(CXXFLAGS)

#block.o: block.cc
#	$(CPP) -c $(CXXFLAGS) block.cc -std=c++11

$(OBJS)/clustering.o: clustering.cc
	$(CPP) $(DBG) -o $@ -c $< $(CXXFLAGS)

$(OBJS)/glpk_linear_solver.o: glpk_linear_solver.cc
	$(CPP) $(DBG) -o $@ -c $< $(CXXFLAGS)

$(OBJS)/solution.o: solution.cc
	$(CPP) $(DBG) -o $@ -c $< $(CXXFLAGS)

$(OBJS)/utils.o: utils.cc
	$(CPP) $(DBG) -o $@ -c $< $(CXXFLAGS)

$(OBJS)/inputdata.o: inputdata.cc
	$(CPP) $(DBG) -o $@ -c $< $(CXXFLAGS)

$(OBJS)/debug.o: debug.cc
	$(CPP) $(DBG) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf $(OBJS)/*.o a.out