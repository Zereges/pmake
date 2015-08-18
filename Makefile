CFLAGS=-c -std=c++11 -g -Wall
LFLAGS=-o pmake

pmake: pmake.cpp main.cpp
	g++ $(LFLAGS) pmake.o main.o

pmake.cpp: src/pmake.hpp src/pmake_options.hpp
	g++ $(CFLAGS) src/pmake.cpp

main.cpp: src/main.hpp src/pmake.hpp src/pmake_options.hpp
	g++ $(CFLAGS) src/main.cpp

clean:
	rm -f *.o
