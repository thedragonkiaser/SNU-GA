.SUFFIXES : .cpp .o

TARGET = packing
OBJECTS = GridSolver.o GridHelper.o CmdLine.o Utililty.o GA.o Selection.o Crossover.o Mutation.o Replacement.o
SRCS = $(OBJECTS:.o=.cpp)
CXX = g++
CXXFLAGS = -O3 -D linux -D _TEST_ -std=c++0x

all: $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)

.cpp.o:
	$(CXX) -c $< $(CXXFLAGS)

run: $(TARGET)
	./$(TARGET) -i packing.in.1 -o packing.out.1 -t 60000 -n 100 -k 1 -plot 1000 -S 2 2.0 1.0 1 1 -X 1 4 -M 0 0.015 -R 2 

clean:
	rm $(OBJECTS) $(TARGET)

GridSolver.o: GridSolver.cpp GA/GA.h lib/Utililty.h lib/CmdLine.h
GridHelper.o: GridHelper.cpp GridHelper.h
CmdLine.o: lib/CmdLine.cpp lib/CmdLine.h
	$(CXX) -c $< $(CXXFLAGS)
Utililty.o: lib/Utililty.cpp lib/Utililty.h
	$(CXX) -c $< $(CXXFLAGS)
GA.o: GA/GA.cpp GA/GA.h GA/Selection.h GA/Crossover.h GA/Mutation.h GA/Replacement.h
	$(CXX) -c $< $(CXXFLAGS)
Selection.o: GA/Selection.cpp GA/Selection.h GA/GA.h lib/Utililty.h lib/CmdLine.h
	$(CXX) -c $< $(CXXFLAGS)
Crossover.o: GA/Crossover.cpp GA/Crossover.h GA/GA.h lib/Utililty.h lib/CmdLine.h
	$(CXX) -c $< $(CXXFLAGS)
Mutation.o: GA/Mutation.cpp GA/Mutation.h GA/GA.h lib/Utililty.h lib/CmdLine.h
	$(CXX) -c $< $(CXXFLAGS)
Replacement.o: GA/Replacement.cpp GA/Replacement.h GA/GA.h lib/Utililty.h lib/CmdLine.h
	$(CXX) -c $< $(CXXFLAGS)
