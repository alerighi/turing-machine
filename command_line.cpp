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
	"    - `help (?)` : show help message\n";
	
constexpr static unsigned int hash(const char *s, int i = 0) {
	if (s == nullptr)
		return 0;
	return !s[i] ? 5381 : (hash(s, i+1) * 33) ^ s[i];
}

void save_file(const char *filename, const turing_machine &tm) {
	FILE *out = fopen(filename, "w");
	if (out == nullptr)
		throw std::runtime_error(std::string("Cannot open ") + filename + " for writing: " + strerror(errno));
	fprintf(out, "; machine program output\n");
	fprintf(out, "memsize %lu\n", tm.get_tape_length());
	fprintf(out, "initsymbol %c\n", tm.initial_symbol);
	fprintf(out, "; transition function\n");
	for (const turing_machine::instruction &i : tm.program) {
		fprintf(out, "+ %s %c %s %c %c\n",
			tm.get_state_name(i.from_state).c_str(),
			i.symbol_read, 
			tm.get_state_name(i.to_state).c_str(),
			i.symbol_write,
			i.tape_direction == direction::L ? '<' : '>');
	}
	fprintf(out, "; end of file\n");
	fclose(out);
}

void load_file(const char *filename, turing_machine &m, FILE *out) {
	FILE *in = fopen(filename, "r");
	if (in == nullptr)
		throw std::runtime_error(std::string("Cannot open file ") + filename + ": " + strerror(errno));
	char l[1024];
	int i = 0;
	while (fgets(l, sizeof(l), in)) {
		i++;
		try {
			parse_line(l, m, out);
		} catch (const std::exception &e) {
			fprintf(out, "Error at file %s line %d : %s\n", filename, i, e.what());
		}
	}
	fclose(in);
}

void parse_line(char *line, turing_machine &m, FILE *out) {

	tokenizer t(line);
	unsigned long steps, ul;
	char r, w;
	std::string from, to;

	switch (hash(t.next())) {
	case hash("echo"):
		fprintf(out, "%s\n", t.to_end());
		break;
	case hash("quit"): 
	case hash("q"):
		exit(EXIT_SUCCESS);
	case hash("help"):
		fputs(USAGE, out);
		break;
	case hash("read"):
	case hash("<"):
		load_file(t.get_string().c_str(), m, out);
		break;
	case hash("save"):
	case hash(">"):
		save_file(t.get_string().c_str(), m);
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
		fprintf(out, "%s\n", m.get_state(50).c_str());
		break;
	case hash("print_state_full"):
	case hash("psf"):
		fprintf(out, "%s\n", m.get_state().c_str());
		break;
	case hash("print_program"):
	case hash("pp"):
		fprintf(out, "%s\n", m.get_program().c_str());
		break;
	case hash("reset"):
	case hash("R"):
		m.reset();
		fputs("Machine reset\n", out);
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
		fputs("Program cleared\n", out);
		break;
	case hash("run"):
	case hash("r"):
		stop = false;
		while (m.step() && !stop);
		if (!stop)
			fputs("Machine reached halt state\n", out);
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

	static constexpr const char *PROMPT = "cmd> ";
	
	char line[1024];
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
		printf(PROMPT);
	while (fgets(line, sizeof(line), stdin)) {
		try {
			parse_line(line, m, stdout);
		} catch (const std::exception &e) {
			fprintf(stdout, "Error: %s\n", e.what());
		}
		if (!piped)
			printf(PROMPT);
	}
	exit(EXIT_SUCCESS);
}
