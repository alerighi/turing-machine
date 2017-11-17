#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <string>
#include <vector>
#include <array>
#include <map>

enum class direction {L, R};

class turing_machine {

	static constexpr const int HALT_STATE = 0;
	static constexpr const int INIT_STATE = 1; 
	static constexpr const char * halt_state_name = "!";
	static constexpr const char * init_state_name = "$";

	struct transition {
		bool is_valid = false;
		int to_state;
		direction dir;
		char write_char;		
	};

	struct instruction {
		int from_state;
		char symbol_read;
		int to_state;
		char symbol_write;
		direction tape_direction;
	};
	
	// machine variables
	char *tape = nullptr;
	unsigned long tape_length;
	int head_pos;
	char initial_symbol;
	int current_state;
	int computation_steps;
	bool is_halt;

	// machine instructions
	std::vector<instruction> instructions;
	std::vector<std::array<transition, 128> > transitions; 

	// state codification variables
	std::vector<std::string> state_code_to_name = {halt_state_name, init_state_name};
	std::map<std::string, int> name_to_state_code = {
		{halt_state_name, HALT_STATE}, 
		{init_state_name, INIT_STATE}
	};

	int get_state_code(const std::string &name);
	int get_number_of_states() const;
	
	std::string get_state_name(int code) const;

public:

	turing_machine(unsigned long memory_size = 1000, char initial_symbol = '0');
	~turing_machine();

	void add_instruction(const std::string &from, char read, const std::string &to, char write, direction dir);
	void del_instruction(int index);
	void clear_program();
	void reset();
	void set_memory_size(unsigned long memory_size);
	void position_head(unsigned long pos) ;
	void set_tape(unsigned long pos, char *str);
	void set_initial_symbol(char init);
	bool step();

	std::string get_tape() const;
	std::string get_tape(int n) const;
	std::string to_string(int n = -1) const;
	std::string print_program() const ;
};

#endif