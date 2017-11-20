/*
 * Turing Machine simulator
 * (c) 2017 Alessandro Righi - MIT license 
 */

#include <csignal>
#include <cstring>

#include <stdexcept>
#include <iostream>
#include <fstream>

#include "command_line.hpp"
#include "turing_machine.hpp"
#include "tokenizer.hpp"

#ifdef UNIX 
#include <unistd.h>
#endif 

#ifdef HAS_GUI
#include "ncurses_gui.hpp"
#endif

bool stop = false;

static void sigint_handler(int /* unused */) {
	signal(SIGINT, sigint_handler);
	stop = true;
}

const static char * USAGE = 
	"    - `load (<) [path]` : load program from file\n"
	"    - `save (>) [path]` : save the current program to file\n"
	"    - `run (r)` : execute the machine till it goes to a halt state\n"
	"    - `step (s) [nsteps]` : execute `nsteps` computations steps. Default 1.\n"
	"    - `memorysize [nbytes]` : set the size of the tape to `nbytes`\n"
	"    - `initialsymbol [symbol]` : set the initla symbol for the tape\n"
	"    - `set_tape [start] [string]` : put `string` on the tape starting from `start`\n"
	"    - `set_state [state]` : set the state to `state`\n"
	"    - `move_head [pos]` : move the head to pos\n"
	"    - `add (+) [from] [read] [to] [write] [dir]` : add a new instruction. From `from` if you read `read` go to `to`, write `write` and move the head to `dir`. `dir` is `<` for left and `>` for right.\n"
	"    - `del (-) [n]` : deletes the instruction number `n``\n"
	"    - `print_program (pp)` : print the program\n"
	"    - `print_state (ps)` : print the machine state\n" 
	"    - `print_state_full (psf)` : print the state showing the whole tape\n"
	"    - `clear (C)` : clears the program\n"
	"    - `reset (R)` : reset the machine\n"
	"    - `echo [string]` : prints `string`\n"
	"    - `quit (q)` : quit\n"
	"    - `help (?)` : show help message";
	
constexpr static unsigned int hash(const char *s, int i = 0) {
	if (s == nullptr)
		return 0;
	return !s[i] ? 5381 : (hash(s, i+1) * 33) ^ s[i];
}

void save_file(const std::string& filename, const turing_machine& tm) {
	std::ofstream out(filename);
	if (!out.is_open())
		throw std::runtime_error("Cannot open file " + filename + " for writing");
	out << "; machine program output\n";
	out << "memsize " << tm.get_tape_length() << '\n';
	out << "initsymbol " << tm.initial_symbol << '\n';
	out << "; transition function\n";
	for (const turing_machine::instruction &i : tm.program) {
		out << "+ ";
		out << tm.get_state_name(i.from_state) << ' ';
		out << i.symbol_read << ' ', 
		out << tm.get_state_name(i.to_state) << ' ',
		out << i.symbol_write << ' ',
		out << (i.tape_direction == direction::L ? '<' : '>') << '\n';
	}
	out << "; end of file\n";
}

void load_file(const std::string& filename, turing_machine &m, std::ostream& out) {
	std::ifstream in(filename);
	if (!in.is_open()) 
		throw std::runtime_error("Error opening file " + filename + " for reading");
	std::string line;
	int i = 0;
	while(std::getline(in, line)) {
		try {
			parse_line(line, m, out);
		} catch (const std::exception &e) {
			out << "Error at file " << filename << " line " << i << " : " <<  e.what() << std::endl;
		}
	}
}

void parse_line(const std::string& line, turing_machine &m, std::ostream& out) {

	tokenizer t(line);
	unsigned long steps, ul;
	char r, w;
	std::string from, to;

	switch (hash(t.next())) {
	case hash("echo"):
		out << t.to_end() << std::endl;
		break;
	case hash("quit"): 
	case hash("q"):
		exit(EXIT_SUCCESS);
	case hash("help"):
		out << USAGE << std::endl;
		break;
	case hash("read"):
	case hash("<"):
		load_file(t.get_string(), m, out);
		break;
	case hash("save"):
	case hash(">"):
		save_file(t.get_string(), m);
		break;
	case hash("step"):
	case hash("s"):
		try {
			steps = t.get_ulong();
		} catch(const std::exception &e) {
			steps = 1;
		}
		while (steps-- && m.step());
		break;
	case hash("memsize"):
	case hash("memorysize"):
		m.set_memory_size(t.get_ulong());
		break;
	case hash("initsymbol"):
	case hash("initialsymbol"):
		m.set_initial_symbol(t.get_symbol());
		break;
	case hash("head_position"):
	case hash("move_head"):
		m.set_head_position(t.get_ulong());
		break;
	case hash("set_tape"): 
		ul = t.get_ulong();
		m.set_tape(ul, t.next());
		break;
	case hash("set_state"):
		m.set_state(t.get_string());
		break;
	case hash("print_state"):
	case hash("ps"):
		out << m.get_state(50) << std::endl;
		break;
	case hash("print_state_full"):
	case hash("psf"):
		out << m.get_state() << std::endl;
		break;
	case hash("print_program"):
	case hash("pp"):
		out << m.get_program() << std::endl;
		break;
	case hash("reset"):
	case hash("R"):
		m.reset();
		out << "Machine reset" << std::endl;
		break;
	case hash("add"):
	case hash("+"):
		from = t.get_string();
		r = t.get_symbol();
		to = t.get_string();
		w = t.get_symbol();
		m.add_instruction(from, r, to, w, t.get_direction());
		break;
	case hash("del"):
	case hash("-"):
		m.del_instruction(t.get_ulong());
		break;
	case hash("clear"):
	case hash("C"):
		m.clear_program();
		out << "Program cleared" << std::endl;
		break;
	case hash("run"):
	case hash("r"):
		stop = false;
		while (m.step() && !stop);
		if (!stop)
			out << "Machine reached halt state" << std::endl;
		break;
#ifdef HAS_GUI
	case hash("gui"):	
		start_gui();
		break;
#endif
	case 0: // null command
		return;
	default:
		throw std::runtime_error("Command not found");
	}	
}

int main(int argc, char *argv[]) 
{
	signal(SIGINT, sigint_handler);

	static const std::string PROMPT = "cmd> ";
	
	bool piped = false;
	turing_machine m;

#ifdef HAS_GUI
	if (argc >= 2 && !strcmp(argv[1], "-gui")) {
		start_gui();
		return 0;
	}
#endif

#ifdef UNIX
	piped = !isatty(fileno(stdin));
#endif

	if (!piped)
		std::cout << PROMPT;
	std::string line;
	while (std::getline(std::cin, line)) {
		try {
			parse_line(line, m, std::cout);
		} catch (const std::exception &e) {
			std::cout << "Error: " << e.what() << std::endl;
		}
		if (!piped)
			std::cout << PROMPT;
	}
	exit(EXIT_SUCCESS);
}
