# paths
SRC = src
INCLUDE = include
MOD = modules

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί
CPPFLAGS = -std=c++11 -Wall -g -I$(INCLUDE)

sniffer: sniffer.o manageWorkers.o common.o worker 
	g++ -o sniffer sniffer.o manageWorkers.o common.o

worker: worker.o createFile.o location.o common.o
	g++ -o worker worker.o createFile.o location.o common.o

sniffer.o: $(SRC)/sniffer.cpp
	g++ $(CPPFLAGS) -c $(SRC)/sniffer.cpp

worker.o: $(SRC)/worker.cpp 
	g++ $(CPPFLAGS) -c $(SRC)/worker.cpp

manageWorkers.o: $(MOD)/manageWorkers.cpp
	g++ $(CPPFLAGS) -c $(MOD)/manageWorkers.cpp

common.o: $(MOD)/common.cpp
	g++ $(CPPFLAGS) -c $(MOD)/common.cpp

createFile.o: $(MOD)/createFile.cpp
	g++ $(CPPFLAGS) -c $(MOD)/createFile.cpp

location.o: $(MOD)/location.cpp
	g++ $(CPPFLAGS) -c $(MOD)/location.cpp



clean:
	rm -f sniffer.o worker.o common.o manageWorkers.o createFile.o location.o sniffer worker common manageWorkers createFile location
	rm -f results/*
	rm -f fifos/*

run: sniffer
	rm -f fifos/*
	rm -f results/*
	./sniffer -p test


