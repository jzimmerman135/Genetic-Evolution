#
# Makefile for evolution sim
#
CXX      = clang++ -O3
CXXFLAGS = -Wall -Wextra -Wreturn-type -g
LDFLAGS  = -g

evolve: main.o grid.o creature.o driver.o
	${CXX} -o $@ $^

test:
	g++ -o test unit_tests.cpp
	
%.o: %.cpp ${INCLUDES}
	${CXX} ${CXXFLAGS} -c $<

valgrind:
	valgrind --leak-check=yes ./evolve

clean:
	rm -rf evolve *.o *.dSYM

