/*
 * Turing Machine simulator
 * (c) 2017 Alessandro Righi - MIT license 
 */

#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <cstdio>

#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif 

#include "turing_machine.hpp"
#include "tokenizer.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;


bool stop = false;

void sigint_handler(int s) {
	signal(SIGINT, sigint_handler);
	stop = true;
}

void usage() {
	cout << "Available commands:" << endl;
	cout << "    - quit (q) =>  close program" << endl;
	cout << "    - help (?) => this help" << endl;
	cout << "    - source (<) <filename> => loads <filename>" << endl;
	cout << "    - step (s) [number of steps] => execute ont (N) steps" << endl;
	cout << "    - run => run the machine to halt (break with CTRL-C one time)" << endl;
	cout << "    - clear (C) => clears the program" << endl;
	cout << "    - add (+) <src>, <read>, <dest>, <write>, <{<,>}> => add instruction" << endl;
	cout << "    - del (-) <n> => delete instruction n" << endl;
	cout << "    - reset (r) => reset the machine" << endl;
	cout << "    - print (pp) => print the program" << endl;
	cout << "    - state (ps) => print the machine state" << endl;
	cout << "    - state_full (psf) => print the machine state, full tape" << endl;
	cout << "    - memsize <size> => set the size of memory" << endl;
	cout << "    - initsymbol <symbol> => set the initial symbol" << endl;
}
	
constexpr static unsigned int hash(const char *s, int i = 0) {
	if (s == nullptr)
		return 0;
	return !s[i] ? 5381 : (hash(s, i+1) * 33) ^ s[i];
}

void parse_line(char*, turing_machine&);

void load_file(const std::string &filename, turing_machine &m) {
	FILE *fp = fopen(filename.c_str(), "r");
	if (!fp)
		throw std::runtime_error(std::string("Cannot open file ") + filename + ": " + strerror(errno));
	char l[1024];
	int i = 0;
	while (fgets(l, sizeof(l), fp)) {
		i++;
		try {
			parse_line(l, m);
		} catch (const std::exception &e) {
			cout << "Error at file " << filename << " line " << std::to_string(i) << ":" << e.what() << endl;
		}
	}
}

void parse_line(char *line, turing_machine &m) {

	tokenizer t(line);
	unsigned long steps, ul;
	char r, w;
	std::string from, to;

	switch (hash(t.next())) {
	case hash("echo"):
		cout << t.to_end() << endl;
		break;
	case hash("quit"): 
	case hash("q"):
	case hash("%%"):
		exit(EXIT_SUCCESS);
	case hash("help"):
		usage();
		break;
	case hash("read"):
	case hash("<"):
		load_file(t.get_string(), m);
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
		m.set_memory_size(t.get_ulong());
		break;
	case hash("initsymbol"):
		m.set_initial_symbol(t.get_symbol());
		break;
	case hash("head_position"):
	case hash("hp"):
	case hash("@@"):
		m.position_head(t.get_ulong());
		break;
	case hash("set_tape"): 
	case hash("W"):
		ul = t.get_ulong();
		m.set_tape(ul, t.next());
		break;
	case hash("state"):
	case hash("ps"):
		cout << m.to_string(50) << endl;
		break;
	case hash("state_full"):
	case hash("psf"):
		cout << m.to_string() << endl;
		break;
	case hash("print"):
	case hash("pp"):
		cout << m.print_program() << endl;
		break;
	case hash("reset"):
		m.reset();
		cout << "Machine reset" << endl;
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
		m.clear_program();
		cout << "Program cleared" << endl;
		break;
	case hash("run"):
		stop = false;
		while (m.step() && !stop);
		if (!stop)
			cout << "Machine reached halt state" << endl;
		break;
	case 0: // null command
		return;
	default:
		throw std::runtime_error("Command not found");
	}	
}

int main(int argc, char *argv[]) 
{
	signal(SIGINT, sigint_handler);

	static constexpr const char *PROMPT = "cmd> ";
	
	char line[1024];
	bool piped = false;

	turing_machine m;

#if defined(__unix__) || defined(__APPLE__)
	piped = !isatty(fileno(stdin));
#endif

	if (!piped)
		printf(PROMPT);
	while (fgets(line, sizeof(line), stdin)) {
		try {
			parse_line(line, m);
		} catch (const std::exception &e) {
			cerr << "Error: " << e.what() << endl;
		}
		if (!piped)
			printf(PROMPT);
	}
	exit(EXIT_SUCCESS);
}
