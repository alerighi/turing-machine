#include "turing_machine.hpp"

#include <cstdlib>
#include <stdexcept>

int turing_machine::get_state_code(const std::string &name) {
	if (name_to_state_code.count(name))
		return name_to_state_code[name];
	int code = name_to_state_code[name] = state_code_to_name.size();
	state_code_to_name.push_back(name);
	return code;
}

std::string turing_machine::get_state_name(int code) const {
	return state_code_to_name[code];
}

int turing_machine::get_number_of_states() const {
	return state_code_to_name.size();
}

turing_machine::turing_machine(unsigned long memory_size, char initial_symbol) : tape_length(memory_size), initial_symbol(initial_symbol) {
	set_memory_size(tape_length);
	reset();
}

turing_machine::~turing_machine() {
	delete[] tape;
}

void turing_machine::add_instruction(const std::string &from, char read, const std::string &to, char write, direction dir) {
	int code_from = get_state_code(from);
	int code_to = get_state_code(to);
	
	instruction i = {code_from, read, code_to, write, dir};
	instructions.push_back(i);


	if (get_number_of_states() > transitions.size()) {
		transitions.resize(get_number_of_states());
	}

	transition &t = transitions.at(code_from)[i.symbol_read];
	t.is_valid = true;
	t.to_state = code_to;
	t.write_char = i.symbol_write;
	t.dir = i.tape_direction;
}

void turing_machine::del_instruction(int index) {
	const instruction &i = instructions.at(index);
	transitions[i.from_state][i.symbol_read].is_valid = false;
	instructions.erase(instructions.begin() + index - 1);
}

void turing_machine::clear_program() {
	instructions.clear();
	transitions.clear();
}

void turing_machine::reset() {
	if (tape == nullptr)
		set_memory_size(tape_length);

	for (int i = 0; i < tape_length; i++) {
		tape[i] = initial_symbol;
	}

	computation_steps = 0; 
	current_state = INIT_STATE;
	is_halt = false;
}

void turing_machine::set_memory_size(unsigned long memory_size) {
	tape_length = memory_size;
	if (tape) 
		delete[] tape;
	try {
		tape = new char[tape_length];
	} catch(const std::exception &e) {
		tape = nullptr;
		throw std::runtime_error("Error allocating memory!");
	}
	head_pos = tape_length / 2;
	reset();
}

void turing_machine::position_head(unsigned long pos) {
	head_pos = pos;
}

void turing_machine::set_tape(unsigned long pos, char *str) {
	while (*str) {
		tape[pos++] = *str++;
	}
	head_pos = pos - 1;
}

void turing_machine::set_initial_symbol(char init) {
	initial_symbol = init;
	reset();
}

bool turing_machine::step() {

	if (is_halt) 
		throw std::runtime_error("The machine is halted");
	
	// increase number of steps
	computation_steps++;

	// read a character from tape
	char c = tape[head_pos];

	// get next transition
	transition next = transitions[current_state][c];

	// check if instruction is valid
	if (!next.is_valid)
		next = transitions[current_state]['-'];
	if (!next.is_valid) {
		is_halt = true;
		throw std::runtime_error("Illegal instruction");
	}

	// write new char to tape
	if (next.write_char == '-')
		tape[head_pos] = c;
	else 
		tape[head_pos] = next.write_char;
	
	// move tape head
	switch (next.dir) {
		case direction::L: head_pos--; break;
		case direction::R: head_pos++; break;
	}

	// check if out of bound
	if (head_pos < 0 || head_pos >= tape_length) {
		is_halt = true;
		throw std::runtime_error("Out of memory");
	}

	// transition to next state
	current_state = next.to_state;

	// check if halted
	if (current_state == HALT_STATE) {
		is_halt = true;
		return false;
	}
	
	return true;
}

std::string turing_machine::get_tape() const {
	if (head_pos >= 0 && head_pos < tape_length) {
		return std::string(tape, head_pos)
			+ '<' + tape[head_pos] + '>'
			+ std::string(tape+head_pos+1, tape_length-head_pos-1);
	}
	if (head_pos < 0) {
		return std::string("<>") + std::string(tape, tape_length);
	} else {
		return std::string(tape, tape_length) + "<>";
	}
}

std::string turing_machine::get_tape(int n) const {
	if (n == -1)
		return get_tape();

	int min = head_pos - n;
	int max = head_pos + n;
	std::string result = "";

	if (min < 0)
		min = 0; 
	if (max > tape_length)
		max = tape_length;

	if (head_pos > 0)
		result += std::to_string(min) + "x[...]" + std::string(tape+min, head_pos-min);
	result += "<";
	if (head_pos >= 0 && head_pos < tape_length)
		result += tape[head_pos];
	result += ">";
	if (head_pos < (static_cast<int>(tape_length) - 1))
		result += std::string(tape+head_pos+1, max-head_pos) + "[...]x" + std::to_string(tape_length-max);

	return result;
}

std::string turing_machine::to_string(int n) const {
	std::string res = "Current state: " + get_state_name(current_state) + "\n";
	res += "Head position: " + std::to_string(head_pos) + "\n";
	res += "Computation steps: " + std::to_string(computation_steps) + "\n";
	res += "Tape state: " + get_tape(n) + "\n";
	return res;
}

std::string turing_machine::print_program() const {
	std::string result = "";

	int l = 0;
	for (const instruction &i : instructions) {
		char num[10];
		snprintf(num, sizeof(num), "%4d", ++l);
		result += std::string(num) + ": ";
		if (i.from_state == INIT_STATE)
			result += " ===> ";
		else if (i.to_state == HALT_STATE) 
			result += " HALT ";
		else 
			result += "      ";	
		result += "(" + get_state_name(i.from_state) + ", ";
		result += std::to_string(i.symbol_read) + ") -> ("; 
		result += get_state_name(i.to_state) + ", "; 
		result += std::to_string(i.symbol_write) + ", ";
		result += std::to_string(i.tape_direction == direction::L ? '<' : '>') + ")\n";
	}

	return result;
}
