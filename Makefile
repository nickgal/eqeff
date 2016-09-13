CC=g++
CFLAGS=-std=c++11 -Wall -Werror

all:
	$(CC) $(CFLAGS) eqeff.cpp -o eqeff
	
