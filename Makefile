# Use the GNU C/C++ compiler:
CC = gcc
CPP = g++

# COMPILER OPTIONS:

CFLAGS = -c

#OBJECT FILES
OBJS = test.o

tester: test.o
	${CPP} -lm ${OBJS} -o tester
test.o: test.cpp
clean:
	rm -f *.o tester
realclean:
	rm -f *.o tester *~ *.swp
