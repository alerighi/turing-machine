CXX=g++
CXXFLAGS=-O2 -std=c++14 -g
LDFLAGS=-lncurses
EXE=TM
OBJECTS=tokenizer.o turing_machine.o command_line.o ncurses_gui.o ncurses_wrapper.o 
HEADERS=tokenizer.hpp turing_machine.hpp ncurses_gui.hpp ncurses_wrapper.hpp

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(OBJECTS) $(EXE)
	