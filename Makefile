CXXFLAGS=-c -std=c++11 -Wall
LDFLAGS=-o pmake -lpthread
WITH-DEBUG=-g

pmake: pmake.o main.o
	$(CXX) $(LDFLAGS) pmake.o main.o

pmake.o: src/pmake.cpp src/main.hpp src/pmake.hpp src/makefile_record.hpp src/pmake_options.hpp src/file.hpp src/makefile_records.hpp
	$(CXX) $(CXXFLAGS) $(WITH-DEBUG) src/pmake.cpp

main.o: src/main.cpp src/main.hpp src/pmake.hpp src/pmake_options.hpp src/makefile_records.hpp src/thread.hpp
	$(CXX) $(CXXFLAGS) $(WITH-DEBUG) src/main.cpp

rebuild: clean pmake

clean:
	$(RM) *.o

install: pmake
	echo symlink to /usr/bin
