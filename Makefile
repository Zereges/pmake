CFLAGS=-c -std=c++11 -g -Wall
LFLAGS=-o pmake

pmake: pmake.o main.o
	g++ $(LFLAGS) pmake.o main.o

pmake.o: src/pmake.cpp src/main.hpp src/pmake.hpp src/makefile_record.hpp src/pmake_options.hpp src/file.hpp src/makefile_records.hpp
	g++ $(CFLAGS) src/pmake.cpp

main.o: src/main.cpp src/main.hpp src/pmake.hpp src/pmake_options.hpp src/makefile_records.hpp
	g++ $(CFLAGS) src/main.cpp

clean:
	rm -f *.o
