CC = g++

build: main.cpp
	$(CC) -std=c++17 -o main main.cpp -lncurses
	./main