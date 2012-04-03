CXX=g++
DEST=./bin
BIN=tsp
OBJS=TSP.o TSPSolver.o CmdLine.o
OPTIONS=-O3 -std=c++0x
DEF=-D linux -D _TEST_

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) -o $(DEST)/$(BIN) $(OBJS)
			  
TSP.o: TSPSolver/TSP.cpp
	$(CXX) -c $(OPTIONS) $(DEF) TSPSolver/TSP.cpp

TSPSolver.o: TSPSolver/TSPSolver.cpp
	$(CXX) -c $(OPTIONS) $(DEF) TSPSolver/TSPSolver.cpp

CmdLine.o: lib/CmdLine.cpp
	$(CXX) -c $(OPTIONS) $(DEF) lib/CmdLine.cpp

clean:
	rm $(OBJS)
	rm $(DEST)/$(BIN)

run:
	$(DEST)/$(BIN) -i cycle.in.20 -o cycle.out.20 -n 50 -k 1 -S 0 4 -X 2 -M 3 0.001 -R 2

