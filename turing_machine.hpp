#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <string>
#include <vector>
#include <array>
#include <map>

enum class direction {L, R};

class turing_machine {

	static const int HALT_STATE;
	static const int INIT_STATE; 
	static const char * halt_state_name;
	static const char * init_state_name;

	struct instruction {
		bool is_valid;
		int from_state;
		char symbol_read;
		int to_state;
		char symbol_write;
		direction tape_direction;
	};
	
	// machine variables
	char *tape = nullptr;
	long tape_length;
	long head_pos;
	char initial_symbol;
	int current_state;
	int computation_steps;
	bool is_halt;

	// machine instructions
	std::vector<instruction> program;
	std::vector<std::array<instruction, 128> > table; 

	// state codification variables
	std::vector<std::string> state_name = {halt_state_name, init_state_name};
	std::map<std::string, int> state_code = {
		{halt_state_name, HALT_STATE}, 
		{init_state_name, INIT_STATE}
	};

	// state codifications functions
	int get_state_code(const std::string &name);
	std::string get_state_name(int code) const;
	const std::string format_instruction(const instruction &i, int line) const;

public:
	turing_machine(unsigned long memory_size = 1000, char initial_symbol = '0');
	~turing_machine();

	// program manipulation instructions
	void add_instruction(const std::string &from, char read, const std::string &to, char write, direction dir);
	void del_instruction(int index);
	void clear_program();
	
	// machine settings
	void set_memory_size(unsigned long memory_size);
	void set_initial_symbol(char init);
	void set_head_position(unsigned long pos) ;
	void set_tape(unsigned long pos, char *str);
	void set_tape(unsigned long pos, char c);
	void set_state(const std::string &state);

	// machine control 
	void reset();
	bool step();
	void move_head(int diff);

	// state getters
	const char * get_tape_raw() const;
	unsigned long get_tape_lenght() const;
	long get_head_pos() const;
	const char * get_current_state() const; 
	int get_computation_steps() const;
	const std::vector<const std::string> get_program_lines() const;
	const std::string get_tape(int n = -1) const;
	const std::string get_state(int n = -1) const;
	const std::string get_program() const;

	friend void save_file(const char *filename, const turing_machine &tm);
};

#endif
