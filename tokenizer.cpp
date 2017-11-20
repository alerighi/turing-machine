#include "tokenizer.hpp"

tokenizer::tokenizer(const std::string& line) : line(line) {
	for (end = 0; end < line.size() && line[end] != '\n' && line[end] != '\t' && line[end] != ';'; end++);

	pos = line.find_first_not_of(' ', 0); 

	if (pos == std::string::npos || pos >= end)
		throw std::runtime_error("Error creating tokenizer");
}

std::string tokenizer::next_string() {
	if (pos == end || pos == std::string::npos)
		throw std::runtime_error("No more tokens");

	size_t last = line.find(' ', pos);
	size_t start = pos;

	if (last == std::string::npos || last > end) {
		pos = end; 
		return line.substr(start, end - start);
	}

	pos = line.find_first_not_of(' ', last); 
	return line.substr(start, last - start);
}

bool tokenizer::check_symbol(char c) {
	return (c >= 'a' && c <= 'z') 
		|| (c >= 'A' && c <= 'Z') 
		|| (c >= '0' && c <= '9') 
		|| c == '$' || c == '-' 
		|| c == '_' || c == '!' 
		|| c == '*' || c == '.';
}

char tokenizer::next_char() {
	return next_string()[0];
}

char tokenizer::next_symbol() {
	char c = next_char();
	if (!check_symbol(c))
		throw std::runtime_error(std::string("Invalid character symbol: ") + c);
	return c;
}

std::string tokenizer::to_end() {
	size_t begin = pos; 
	pos = end;
	return line.substr(begin, end - begin);
}

direction tokenizer::next_direction() {
	switch (next_char()) {
		case '<': return direction::L;
		case '>': return direction::R;
		default: throw std::invalid_argument("Syntax error: invalid direction character");
	}
}

unsigned long tokenizer::next_ulong() {
	const char *str = next_string().c_str();
	char *endp;
	if (str != nullptr) {
		long result = strtol(str, &endp, 10);
		if (*endp == '\0')
			return result;
	}
	throw std::runtime_error("Invalid numeric constant");
}
