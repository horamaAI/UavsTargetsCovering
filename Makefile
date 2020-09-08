# Compilator
CPP = g++

# Options of compilation
CFLAGS = -g -Wall  -lglpk -ligraph -lm

# Build target executable
TARGET = srcs/main

all: $(TARGET).cc
	$(CPP) -o a.out $(TARGET).cc $(CFLAGS)

clean:
	rm $(TARGET)