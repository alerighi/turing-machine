#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include "turing_machine.hpp"

class tokenizer {
	char *line;
	char *end;

public:
	tokenizer(char *line);

	char * next();
	char * to_end() const;

	char get_char();
	char get_symbol();
	std::string get_string();
	direction get_direction();
	unsigned long get_ulong();

	static bool check_symbol(char c);
};

#endif