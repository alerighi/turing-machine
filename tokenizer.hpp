#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include "turing_machine.hpp"

class tokenizer {
	char *str;
	char *line;
	char *end;

	void init();
	
public:
	tokenizer(char *line);
	tokenizer(const std::string& s);
	~tokenizer();
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
