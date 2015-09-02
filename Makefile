CXXFLAGS=-c -std=c++11 -Wall
LDFLAGS=-o pmake -lpthread
WITH-DEBUG=-g

pmake: pmake.o main.o makefile_record.o
	$(CXX) $(LDFLAGS) $+

pmake.o: src/pmake.cpp src/pmake.hpp src/makefile_record.hpp src/file.hpp src/pmake_options.hpp src/main.hpp
	$(CXX) $(CXXFLAGS) $(WITH-DEBUG) $<

main.o: src/main.cpp src/main.hpp src/pmake.hpp src/pmake_options.hpp
	$(CXX) $(CXXFLAGS) $(WITH-DEBUG) $<

makefile_record.o: src/makefile_record.cpp src/makefile_record.hpp src/file.hpp src/pmake_options.hpp
	$(CXX) $(CXXFLAGS) $(WITH-DEBUG) $<

rebuild: clean pmake

clean:
	$(RM) *.o

install: pmake
    cp pmake /usr/bin/pmake
