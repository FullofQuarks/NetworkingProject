#Makefile for Networking Project
CC=g++
CFLAGS=-c -std=c++11

all: part1 part2 part3

part1: host.o
	$(CC) -o host host.o

host.o: host.cpp
	$(CC) $(CFLAGS) host.cpp

part2: bridge.o
	$(CC) -o bridge bridge.o

bridge.o: bridge.cpp
	$(CC) $(CFLAGS) bridge.cpp

part3: router.o
	$(CC) -o router router.o

router.o: router.cpp
	$(CC) $(CFLAGS) router.cpp
clean:
	rm host.o bridge.o router.o