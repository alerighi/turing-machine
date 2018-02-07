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
#include <getopt.h>
#endif 

#ifdef HAS_GUI
#include "ncurses_gui.hpp"
#endif

bool stop = false;

const static char * USAGE = 
	"    - load (<) [path] : load program from file\n"
	"    - save (>) [path] : save the current program to file\n"
	"    - run (r) : execute the machine till it goes to a halt state\n"
	"    - step (s) [nsteps] : execute `nsteps` computations steps. Default 1.\n"
	"    - memorysize [nbytes] : set the size of the tape to `nbytes`\n"
	"    - initialsymbol [symbol] : set the initla symbol for the tape\n"
	"    - set_tape [start] [string] : put `string` on the tape starting from `start`\n"
	"    - set_state [state] : set the state to `state`\n"
	"    - move_head [pos] : move the head to pos\n"
	"    - add (+) [from] [read] [to] [write] [dir] : add a new instruction. From `from` if you read `read` go to `to`, write `write` and move the head to `dir`. `dir` is `<` for left and `>` for right.\n"
	"    - del (-) [n] : deletes the instruction number `n``\n"
	"    - print_program (pp) : print the program\n"
	"    - print_state (ps) : print the machine state\n" 
	"    - print_state_full (psf) : print the state showing the whole tape\n"
	"    - clear (C) : clears the program\n"
	"    - reset (R) : reset the machine\n"
	"    - echo [string] : prints `string`\n"
	"    - quit (q) : quit\n"
	"    - help (?) : show help message\n"
	"    - gui : start gui mode";

constexpr static unsigned int hash(const char *s, int i = 0) 
{
	if (s == nullptr)
		return 0;
	return !s[i] ? 5381 : (hash(s, i+1) * 33) ^ s[i];
}

static void sigint_handler(int /* unused */) 
{
	signal(SIGINT, sigint_handler);
	stop = true;
}

void save_file(const std::string& filename, const turing_machine& tm) 
{
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

void load_file(const std::string& filename, turing_machine &m, std::ostream& out) 
{
	std::ifstream in(filename);
	if (!in.is_open()) 
		throw std::runtime_error("Error opening file " + filename + " for reading");
	std::string line;
	int i = 1;
	while (std::getline(in, line)) {
		try {
			parse_line(line, m, out);
			i++;
		} catch (const std::exception &e) {
			out << "Error at file " << filename << " line " << i << " : " <<  e.what() << std::endl;
		}
	}
}

void parse_line(const std::string& line, turing_machine &m, std::ostream& out) 
{
	unsigned long steps, ul;
	char r, w;
	std::string from, to, command;
	tokenizer t;

	try {
		t = tokenizer(line);
		command = t.next_string();
	} catch (const std::exception &e) {
		return;
	}
	switch (hash(command.c_str())) {
	case hash("echo"):
		out << t.to_end() << std::endl;
		break;
	case hash("quit"): 
	case hash("exit"):
	case hash("q"):
		exit(EXIT_SUCCESS);
	case hash("help"):
		out << USAGE << std::endl;
		break;
	case hash("read"):
	case hash("<"):
		load_file(t.next_string(), m, out);
		break;
	case hash("save"):
	case hash(">"):
		save_file(t.next_string(), m);
		break;
	case hash("step"):
	case hash("s"):
		try {
			steps = t.next_ulong();
		} catch(const std::exception &e) {
			steps = 1;
		}
		while (steps-- && m.step());
		break;
	case hash("memsize"):
	case hash("memorysize"):
		m.set_memory_size(t.next_ulong());
		break;
	case hash("initsymbol"):
	case hash("initialsymbol"):
		m.set_initial_symbol(t.next_symbol());
		break;
	case hash("head_position"):
	case hash("move_head"):
		m.set_head_position(t.next_ulong());
		break;
	case hash("set_tape"): 
		ul = t.next_ulong();
		m.set_tape(ul, t.next_string());
		break;
	case hash("set_state"):
		m.set_state(t.next_string());
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
		from = t.next_string();
		r = t.next_symbol();
		to = t.next_string();
		w = t.next_symbol();
		m.add_instruction(from, r, to, w, t.next_direction());
		break;
	case hash("del"):
	case hash("-"):
		m.del_instruction(t.next_ulong());
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

#ifdef UNIX

void parse_cmdline(int argc, char *argv[])
{
	struct option long_options[] = {
		{"help", 0, NULL, 'h'},
		{"version", 0, NULL, 'v'},
		{"gui", 0, NULL, 'g'}
	};
	int opt; 
	while ((opt = getopt_long(argc, argv, "hvg", long_options, NULL)) != -1) {
		switch (opt) {
		case 'h':
			std::cout << "Usage: " << argv[0] << " [-vhg]" << std::endl;
			std::cout << "\t-h, --help\tShow this help message" << std::endl;
			std::cout << "\t-v, --version\tShow program version" << std::endl;
			std::cout << "\t-g, --gui\tStart in ncurses gui mode" << std::endl;	
			exit(EXIT_SUCCESS);
		case 'v':
			std::cout << "TM VERSION V 1.0" << std::endl;
			exit(EXIT_SUCCESS);
		case 'g':
#ifdef HAS_GUI
			start_gui();
#endif
		default:
			std::cerr << "Unrecognized option: " << opt << std::endl;
			exit(EXIT_FAILURE);
		}		
	}
}

#endif

int main(int argc, char *argv[]) 
{
	signal(SIGINT, sigint_handler);

	static const std::string PROMPT = "cmd> ";
	
	bool piped = false;
	turing_machine m;

#ifdef UNIX
	parse_cmdline(argc, argv);

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
