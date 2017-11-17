CXX=g++
CXXFLAGS=-O2 -std=c++14
LDFLAGS=
EXE=TM
OBJECTS=tokenizer.o turing_machine.o command_line.o
HEADERS=tokenizer.hpp turing_machine.hpp

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(OBJECTS) $(EXE)
	