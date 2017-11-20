#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include "turing_machine.hpp"

class tokenizer {
	std::string line;	
	size_t pos;
	size_t end;

public:
	tokenizer() = default;
	tokenizer(const std::string& line);

	std::string next_string();
	std::string to_end();
	char next_char();
	char next_symbol();
	direction next_direction();
	unsigned long next_ulong();
	static bool check_symbol(char c);
};

#endif
