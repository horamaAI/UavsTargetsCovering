# Compilator
CPP = g++

# Options of compilation
CFLAGS = -g -Wall  -lglpk -ligraph -lm

# Build target executable
TARGET = srcs/main

all: $(TARGET).cpp
	$(CPP) -o a.out $(TARGET).cpp $(CFLAGS)

clean:
	rm $(TARGET)