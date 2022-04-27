CC = g++
FLAGS = -std=c++11
COMPILE_FLAGS = $(FLAGS) -c


all: meanCalculator class.out average.out std.out

meanCalculator: main.o 
	$(CC) main.o $(FLAGS) -o meanCalculator.out
main.o: main.cpp defines.hpp
	$(CC) $(COMPILE_FLAGS) main.cpp -o main.o
average.out: average.o
	$(CC) average.o $(FLAGS) -o average.out

average.o: average.cpp defines.hpp
	$(CC) $(COMPILE_FLAGS) average.cpp -o average.o

class.out: class.o 
	$(CC) class.o $(FLAGS) -o class.out

class.o: class.cpp defines.hpp
	$(CC) $(COMPILE_FLAGS) class.cpp -o class.o

std.out: std.o
	$(CC) std.o $(FLAGS) -o std.out

std.o: std.cpp defines.hpp
	$(CC) $(COMPILE_FLAGS) std.cpp -o std.o

clean:
	rm -rf *.o
	rm -rf *.out