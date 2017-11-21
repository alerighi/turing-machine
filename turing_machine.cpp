#include "turing_machine.hpp"

#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <cassert> 

const int turing_machine::HALT_STATE = 0;
const int turing_machine::INIT_STATE = 1; 
const char * turing_machine::halt_state_name = "!";
const char * turing_machine::init_state_name = "$";

// constructors
turing_machine::turing_machine(long memory_size, char initial_symbol) 
	: tape(memory_size, initial_symbol), head_pos(initial_symbol/2), initial_symbol(initial_symbol) {
	reset();
}

// state condifications functions
int turing_machine::get_state_code(const std::string &name) {
	if (state_code.count(name))
		return state_code[name];
	int code = state_code.size();
	state_code[name] = code;
	state_name.push_back(name);
	return code;
}

std::string turing_machine::get_state_name(int code) const {
	return state_name[code];
}

// program manipulation
void turing_machine::add_instruction(const std::string &from, char read, const std::string &to, char write, direction dir) {
	int code_from = get_state_code(from);
	int code_to = get_state_code(to);
	
	instruction i = { true, code_from, read, code_to, write, dir };
	program.push_back(i);

	if (state_name.size() > program.size()) {
		table.resize(state_name.size());
	}

	table[code_from][i.symbol_read] = i;
}

void turing_machine::del_instruction(int index) {
	const instruction &i = program[index];
	table[i.from_state][i.symbol_read].is_valid = false;
	program.erase(program.begin() + index - 1);
}

void turing_machine::clear_program() {
	program.clear();
	std::fill(table.begin(), table.end(), std::array<instruction, 128>());
}

// machine settings
void turing_machine::set_memory_size(long memory_size) {
	tape.resize(memory_size);
	head_pos = memory_size / 2;
	reset();
}

void turing_machine::set_head_position(long pos) {
	head_pos = pos;
}

void turing_machine::set_tape(long pos, const std::string &str) {
	tape.replace(pos, str.size(), str);
}

void turing_machine::set_tape(long pos, char c) {
	tape.at(pos) = c; 
}

void turing_machine::set_state(const std::string &state) {
	if (state_code.count(state)) 
		current_state = state_code[state];
	else 
		throw std::runtime_error("Non existent state!");
}


void turing_machine::set_initial_symbol(char init) {
	initial_symbol = init;
	reset();
}

// machine control 
void turing_machine::reset() {
	fill(tape.begin(), tape.end(), initial_symbol);
	computation_steps = 0; 
	current_state = turing_machine::INIT_STATE;
	is_halt = false;
}

bool turing_machine::step() {

	if (is_halt) 
		throw std::runtime_error("The machine is halted");

	if (table.size() < 1) 
		throw std::runtime_error("Program empty!");
	
	// increase number of steps
	computation_steps++;

	// read a character from tape
	char c = tape[head_pos];

	// get next transition
	instruction next = table[current_state][c];

	// check if instruction is valid
	if (!next.is_valid)
		next = table[current_state]['-'];
	if (!next.is_valid) {
		is_halt = true;
		throw std::runtime_error("Illegal instruction");
	}

	// write new char to tape
	if (next.symbol_write == '-')
		tape[head_pos] = c;
	else 
		tape[head_pos] = next.symbol_write;
	
	// move tape head
	switch (next.tape_direction) {
		case direction::L: head_pos--; break;
		case direction::R: head_pos++; break;
	}

	// check if out of bound
	if (head_pos < 0 || head_pos >= get_tape_length()) {
		is_halt = true;
		throw std::runtime_error("Out of memory");
	}

	// transition to next state
	current_state = next.to_state;

	// check if halted
	if (current_state == turing_machine::HALT_STATE) {
		is_halt = true;
		return false;
	}
	
	return true;
}

void turing_machine::move_head(int diff) {
	if (head_pos + diff >= 0 && head_pos + diff < get_tape_length())	
		head_pos += diff;
	else 
		throw std::runtime_error("Head out of bounds");
}

// state getters
const std::string& turing_machine::get_tape_raw() const {
	return tape;
}
long turing_machine::get_tape_length() const {
	return tape.size();
}
long turing_machine::get_head_pos() const {
	return head_pos;
}

const std::string& turing_machine::get_current_state() const {
	return state_name[current_state];
}

int turing_machine::get_computation_steps() const {
	return computation_steps;
}

const std::string turing_machine::get_tape(int n) const {
	if (n == -1) {
		if (head_pos >= 0 && head_pos < get_tape_length()) {
			return tape.substr(0, head_pos)
				+ '<' + tape[head_pos] + '>'
				+ tape.substr(head_pos+1, tape.size()-head_pos-1);
		}
		if (head_pos < 0) {
			return std::string("<>") + tape;
		} else {
			return tape + "<>";
		}
	}

	int min = head_pos - n;
	int max = head_pos + n;
	std::string result = "";

	if (min < 0)
		min = 0; 
	if (max > get_tape_length())
		max = get_tape_length();

	if (head_pos > 0)
		result += std::to_string(min) + "x[...]" + tape.substr(min, head_pos-min);
	result += "<";
	if (head_pos >= 0 && head_pos < get_tape_length())
		result += tape[head_pos];
	result += ">";
	if (head_pos < get_tape_length() - 1)
		result += tape.substr(head_pos+1, max-head_pos) + "[...]x" + std::to_string(get_tape_length()-max);

	return result;
}

const std::string turing_machine::get_state(int n) const {
	std::string res = "Current state: " + get_state_name(current_state) + "\n";
	res += "Head position: " + std::to_string(head_pos) + "\n";
	res += "Computation steps: " + std::to_string(computation_steps) + "\n";
	res += "Tape state: " + get_tape(n) + "\n";
	return res;
}

const std::string turing_machine::format_instruction(const instruction &i, int line) const {
	char num[10];
	snprintf(num, sizeof(num), "%4d", line);
	std::string result = std::string(num) + ": ";
	if (i.from_state == INIT_STATE)
		result += " ===> ";
	else if (i.to_state == HALT_STATE) 
		result += " HALT ";
	else 
		result += "      ";	
	result += "(" + get_state_name(i.from_state) + ", ";
	result += i.symbol_read;
	result += ") -> ("; 
	result += get_state_name(i.to_state) + ", "; 
	result += i.symbol_write;
	result += ", ";
	result += (i.tape_direction == direction::L ? '<' : '>');
	result += ")";
	if (current_state == i.from_state && (tape[head_pos] == i.symbol_read || i.symbol_read == '-'))
		result += " <- ";
	result += "\n";
	return result;
}

const std::string turing_machine::get_program() const {
	std::string result = "";

	int l = 0;
	for (const instruction &i : program) {
		result += format_instruction(i, ++l);
	}

	return result;
}

const std::vector<std::string> turing_machine::get_program_lines() const {
	std::vector<std::string> result;

	for (size_t i = 0; i < program.size(); i++) {
		result.push_back(format_instruction(program[i], i+1));
	}

	return result;
}
