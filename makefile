CXX = g++
FLAGS = -Wextra -Wall --short-enums -std=c++17
ifdef DEBUG
	FLAGS += -DDEBUG -g
else
	FLAGS += -O3
endif

all: solver

solver: solve.o game.o
	$(CXX) $(FLAGS) -o $@ $^

solve.o: solve.cpp solve.hpp game.o
	$(CXX) $(FLAGS) -c -o $@ $<

game.o: game.cpp game.hpp
	$(CXX) $(FLAGS) -c -o $@ $<

clean:
	rm -f solver *.o
