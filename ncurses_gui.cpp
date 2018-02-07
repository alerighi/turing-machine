#include <stdexcept> 
#include <iostream>
#include <sstream>
#include <string>

#include "ncurses_gui.hpp"
#include "turing_machine.hpp"
#include "ncurses_wrapper.hpp"
#include "command_line.hpp"

const std::string TITLE = "Turing Machine simulator";

class tape_window : public ncurses::window {

	const char *tape;
	long head_pos;
	long window_start;
	long tape_length;
	int start; 
	int end;
	int number_of_cells;
	const turing_machine &tm;

public:

	tape_window(int height, int width, int starty, int startx, const turing_machine &tm) : 
		window(height, width, starty, startx + ((width-1) % 4) + 1), number_of_cells((width-1)/4 - 1), tm(tm) 
	{
		update_tape();
	}

	void refresh() 
	{

		move(0, 0);

		addch(ncurses::charcode::ACS_ULCORNER);
		for (int i = 1; i < number_of_cells; i++) {
			addch(ncurses::charcode::ACS_HLINE);
			addch(ncurses::charcode::ACS_HLINE);
			addch(ncurses::charcode::ACS_HLINE);
			addch(ncurses::charcode::ACS_TTEE);
		}
		addch(ncurses::charcode::ACS_HLINE);
		addch(ncurses::charcode::ACS_HLINE);
		addch(ncurses::charcode::ACS_HLINE);
		addch(ncurses::charcode::ACS_URCORNER);

		move(3, 0);
		clrtoeol(); 

		move(1, 0);
		addch(ncurses::charcode::ACS_VLINE);
		for (int i = 0; i < start; i++) {
			addch(' ');
			addch(' ');
			addch(' ');
			addch(ncurses::charcode::ACS_VLINE);
		}
		for (int i = start; i < end; i++) {
			addch(' ');
			//attron(COLOR_PAIR(1));
			addch(tape[window_start+i-start]);
			//attroff(COLOR_PAIR(1));
			if (window_start+i-start == head_pos) {
				move(3, getcurx() - 1);
				addch('^');
				move(1, getcurx());
			}
			addch(' ');
			addch(ncurses::charcode::ACS_VLINE);
		}
		for (int i = end; i < number_of_cells; i++) {
			addch(' ');
			addch(' ');
			addch(' ');
			addch(ncurses::charcode::ACS_VLINE);
		}

		move(2, 0);
		addch(ncurses::charcode::ACS_LLCORNER);
		for (int i = 1; i < number_of_cells; i++) {
			addch(ncurses::charcode::ACS_HLINE);
			addch(ncurses::charcode::ACS_HLINE);
			addch(ncurses::charcode::ACS_HLINE);
			addch(ncurses::charcode::ACS_BTEE);
		}
		addch(ncurses::charcode::ACS_HLINE);
		addch(ncurses::charcode::ACS_HLINE);
		addch(ncurses::charcode::ACS_HLINE);
		addch(ncurses::charcode::ACS_LRCORNER);

		window::refresh();

	}

	void scroll_left() 
	{
		if (window_start > 0)
			window_start--;
		refresh();
	}

	void scroll_right() 
	{
		if (window_start < tape_length - number_of_cells - 1)
			window_start++;
		refresh();
	}

	void update_tape() 
	{
		tape = tm.get_tape_raw().c_str();
		head_pos = tm.get_head_pos();
		tape_length = tm.get_tape_length();

		if (tape_length < number_of_cells) {
			window_start = 0;
			start = (number_of_cells - tape_length) / 2;
			end = start + tape_length;
		} else {
			window_start = head_pos - number_of_cells / 2;
			if (window_start < 0) 
				window_start = 0;
			if (window_start + number_of_cells >= tape_length)
				window_start = tape_length - number_of_cells;
			start = 0;
			end = number_of_cells;
		}
		refresh();
	}

};

class code_window : public ncurses::window {

	const turing_machine &tm;
	unsigned int start = 0; 

public:
	code_window(int height, int width, int starty, int startx, const turing_machine &tm) :
		window(height, width, starty, startx, true), tm(tm) {}

	void update_code() 
	{
		move(0, 0);
		if (tm.get_program_lines().size() == 0) {
			erase();
			refresh();
			return;
		}
		for (size_t i = 0; i < static_cast<size_t>(height) && start + i < tm.get_program_lines().size(); i++) {
			printw("%s", tm.get_program_lines()[start + i].c_str());
			clrtoeol(); 
		}
		clrtobot();
	}

	void scroll_down() 
	{
		if (start + height < tm.get_program_lines().size()) {
			start++;
			update_code();
		}
	}

	void scroll_up() 
	{
		if (start > 0) {
			start--;
			update_code();
		}
	}
};

class machine_status_win : public ncurses::window {

	const turing_machine &tm;

public:
	machine_status_win(int h, int w, int y, int x, const turing_machine &tm) :
		window(h, w, y, x, true), tm(tm) {}

	void update_status() 
	{
		move(0, 0);
		printw("Current state: %s\n", tm.get_current_state().c_str());
		printw("Head position: %d/%d\n", tm.get_head_pos(), tm.get_tape_length());
		printw("Computation steps: %d\n", tm.get_computation_steps());
	}
};

class gui {
	turing_machine m;
	ncurses::window root_win;
	tape_window tape_win;
	ncurses::window cmd_win;
	code_window code_win;
	ncurses::window status_win;
	machine_status_win machine_win;

public:
	gui() : 
		root_win(ncurses::initscr()), 
		tape_win(4, ncurses::get_cols(), 1, 0, m),
		cmd_win(ncurses::get_lines() - 11, ncurses::get_cols()/2, 10, 0, true),
		code_win(ncurses::get_lines() - 6, ncurses::get_cols()/2, 5, ncurses::get_cols()/2, m),
		status_win(1, ncurses::get_cols(), ncurses::get_lines() - 1, 0),
		machine_win(5,  ncurses::get_cols()/2, 5, 0, m)
	{
		ncurses::set_cbreak(true);
		root_win.keypad(true);
		ncurses::set_echo(false);
		ncurses::set_cursor_visible(false);
		root_win.set_title(TITLE.c_str());
		cmd_win.set_title("Command output");
		code_win.set_title("Machine program");
		machine_win.set_title("Machine status");

		root_win.refresh();
		tape_win.refresh();
		cmd_win.set_scroll(true);
		machine_win.update_status();

		m.reset();
	}

	~gui() 
	{
		ncurses::set_cursor_visible(true);
		ncurses::set_echo(true);
		root_win.clear();
		root_win.refresh();
		ncurses::endwin();
	}

	void update() 
	{
		tape_win.update_tape();
		code_win.update_code();
		machine_win.update_status();
	}

	void prompt_command() 
	{
		char line[1024];
		std::stringstream out;

		ncurses::set_echo(true);
		ncurses::set_cursor_visible(true);

		status_win.move(0, 0);
		status_win.printw(": ");
		status_win.clrtoeol();
		if (status_win.getstr(line) != -1) {
			try {
				::parse_line(line, m, out);
				cmd_win.printw("%s", out.str().c_str());			
				update(); 
			} catch (const std::exception &e) {
				cmd_win.printw("Error: %s\n", e.what());
			}
		}

		ncurses::set_echo(false);
		ncurses::set_cursor_visible(false);
	}

	[[noreturn]] void input_loop() 
	{
		while (true) {
			try {
				switch(root_win.getch()) {
				case 'q':
					exit(EXIT_SUCCESS);
				case ncurses::KEY_LEFT:
					tape_win.scroll_left();
					break;
				case ncurses::KEY_RIGHT:
					tape_win.scroll_right();
					break;
				case 'r':
					while (m.step() && !stop);
					update();
					break;
				case 's':
					m.step();
					update();
					break;
				case ':':
					prompt_command();
					break;
				case 'R':
					m.reset();
					update();
					break;
				case '<':
					m.move_head(-1);
					update();
					break;
				case '>':
					m.move_head(1);
					update();
					break;
				case ncurses::KEY_DOWN:
					code_win.scroll_down();
					break;
				case ncurses::KEY_UP:
					code_win.scroll_up();
					break;
				case '\\':
					m.set_tape(m.get_head_pos(), static_cast<char>(root_win.getch()));
					tape_win.update_tape();
				}
			} catch(const std::exception &e) {
				cmd_win.printw("Error %s\n", e.what());
			}
		}
	}
};

[[noreturn]] void start_gui() 
{
	gui g;
	g.input_loop();	
}
