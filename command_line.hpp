#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <cstdio>

#include "turing_machine.hpp"

#if defined(__unix__) || defined(__APPLE__)
#define UNIX
#ifndef NO_GUI
#define HAS_GUI
#include "ncurses_gui.hpp"
#endif
#endif 


extern bool stop;

void parse_line(char *line, turing_machine &tm, FILE *out);
void load_file(const char *filename, turing_machine &m, FILE *out);

#endif