#include "tokenizer.hpp"

tokenizer::tokenizer(char *line) : line(line) {
	init();
}

tokenizer::tokenizer(const std::string &s) : str(strdup(s.c_str())), line(str) {
	init();
}

tokenizer::~tokenizer() {
	if (str != nullptr)
		free(str);
}

void tokenizer::init() {
	char *s = line;
	// Sets the end pointer. The string ends with a newline or a comment character 
	for (; *s; s++) {
		if (*s == '\n' || *s == '\r' || *s == ';' || *s == '#') {
			*s = '\0';
			break;
		}
	}
	end = s;
}

char * tokenizer::next() {
	char *start; 
	if (line >= end)
		return nullptr;
	while (line < end && *line == ' ') 
		line++;
	if (line == end)
		return nullptr;
	start = line;
	while (line < end && *line != ' ')
		line++;
	if (line == nullptr)
		return nullptr;
	*line++ = '\0';
	if (start == line)
		return nullptr;
	return start;
}

bool tokenizer::check_symbol(char c) {
	return (c >= 'a' && c <= 'z') 
		|| (c >= 'A' && c <= 'Z') 
		|| (c >= '0' && c <= '9') 
		|| c == '$' || c == '-' 
		|| c == '_' || c == '!' 
		|| c == '*' || c == '.';
}

char tokenizer::get_char() {
	const char *s = next();
	if (s == nullptr)
		throw std::runtime_error("Parsing error");
	return *s;
}

char tokenizer::get_symbol() {
	char c = get_char();
	if (!check_symbol(c))
		throw std::runtime_error(std::string("Invalid character symbol: ") + c);
	return c;
}

char * tokenizer::to_end() const {
	if (line < end)
		return line;
	else 
		return nullptr;
}

std::string tokenizer::get_string() {
	const char *s = next();
	if (s == nullptr)
		throw std::runtime_error("Parsing error");
	return std::string(s);
}

direction tokenizer::get_direction() {
	switch (get_char()) {
		case '<': return direction::L;
		case '>': return direction::R;
		default: throw std::invalid_argument("Syntax error: invalid direction character");
	}
}

unsigned long tokenizer::get_ulong() {
	char *str = next();
	char *endp;
	if (str != nullptr) {
		long result = strtol(str, &endp, 10);
		if (*endp == '\0')
			return result;
	}
	throw std::runtime_error("Invalid numeric constant");
}
