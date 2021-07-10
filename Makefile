OBJECT = main.o ts.o as.o hg.o cm.o cu.o ss.o utils.o
INCLUDE = ./include
LIBRARY = ./lib
GCC = g++
CPP_FLAGS = -O2 -std=c++11

benchmark : $(OBJECT)
	g++ -o benchmark $(OBJECT) $(CPP_FLAGS)
# add "-lboost_program_options" if use boost lib
main.o : main.cpp
	g++ -c main.cpp -I$(INCLUDE) $(CPP_FLAGS)
ts.o : ts.cpp ts.h sketch.h murmur3.h
	g++ -c ts.cpp -I$(INCLUDE) $(CPP_FLAGS)
cm.o : cm.cpp cm.h sketch.h murmur3.h
	g++ -c cm.cpp -I$(INCLUDE) $(CPP_FLAGS)
cu.o : cu.cpp cu.h sketch.h murmur3.h
	g++ -c cu.cpp -I$(INCLUDE) $(CPP_FLAGS)
as.o : as.cpp as.h sketch.h murmur3.h
	g++ -c as.cpp -I$(INCLUDE) $(CPP_FLAGS)
ss.o : ss.cpp ss.h sketch.h murmur3.h
	g++ -c ss.cpp -I$(INCLUDE) $(CPP_FLAGS)
hg.o : hg.cpp hg.h sketch.h murmur3.h
	g++ -c hg.cpp -I$(INCLUDE) $(CPP_FLAGS)
utils.o : utils.cpp utils.h
	g++ -c utils.cpp $(CPP_FLAGS)
clean :
	find . -name "*.o"  | xargs rm -f
	rm benchmark
