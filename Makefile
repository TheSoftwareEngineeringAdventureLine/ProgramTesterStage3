#-----------------------------------------------------------------------

# Use the GNU C/C++ compiler:
CC = gcc
CXX = g++

# COMPILER OPTIONS:
##CFLAGS = -c

#OBJECT FILES
OBJS = string_ops.o dir_traversal.o test.o

all: test

test: string_ops.o dir_traversal.o test.o
	$(CXX) -lm $^ -O3 -g -o $@
test.o: test.cpp
string_ops.o: string_ops.cpp
dir_traversal.o: dir_traversal.cpp

#-----------------------------------------------------------------------
