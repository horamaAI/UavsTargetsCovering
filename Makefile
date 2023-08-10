# Compilator
CPP		= g++
# Exe name
NAME 	= uas_deplo
# Build target executable
TRGT	= $(NAME)

# Options of compilation
DBG=-g
CXXFLAGS = -O0 -std=c++20 -Wall -Wextra -Wshadow -pedantic -Weffc++ -Werror -DDEBUG

# Additionnal libraries inclusion
LDLIBS = -lglpk -ligraph -lm

# Directories
HEADS_DIR 	:= heads
OBJS_DIR	:= objs
SRCS_DIR	:= srcs
HEADS_FILES	:= block.h clustering.h glpk_linear_solver.h /solution.h utils.h inputdata.h debug.h
HEADS		:= $(addprefix $(HEADS_DIR)/, $(HEADS_FILES))
SRCS_FILES	:= main.cc block.cc clustering.cc glpk_linear_solver.cc solution.cc utils.cc inputdata.cc debug.cc
SRCS		:= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))
OBJS		:= $(SRCS:$(SRCS_DIR)/%.cc=$(OBJS_DIR)/%.o)

$(TRGT): $(OBJS)
	$(CPP) $(DBG) $(LDLIBS) $(CXXFLAGS) -o $(TRGT) $^

all: $(TRGT)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cc
	$(CPP) $(DBG) $(CXXFLAGS) -c $< -o $@ -I.$(HEADS_DIR)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TRGT)

re: fclean all

.PHONY: all clean fclean re
