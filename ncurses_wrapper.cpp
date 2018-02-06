#include <stdexcept>
#include <cstring>
#include <cstdarg>
#include <iostream>

#include "ncurses_wrapper.hpp"

#define NCURSES_NOMACROS
#include <curses.h>

namespace ncurses {

	window initscr() 
	{
		return window(::initscr());
	}

	void endwin() 
	{
		::endwin();
	}

	void set_echo(bool val) 
	{
		if (val)
			::echo();
		else 
			::noecho();
	}

	void set_cbreak(bool val) 
	{
		if (val)
			::cbreak();
		else 
			::nocbreak();
	}

	void set_cursor_visible(bool val) 
	{
		::curs_set(val);
	}

	chtype get_keycode(charcode c) 
	{
		return ::acs_map[static_cast<unsigned int>(c)];
	}

	int get_lines() 
	{
		return ::LINES;
	}

	int get_cols() 
	{
		return ::COLS;
	}

	window::window(WINDOW *window, int h, int w, int y, int x, bool box) : 
		win(window), box(box), height(h), width(w), start_y(y), start_x(x)
	{
		if (win == nullptr) 
			throw std::runtime_error("Error createing window");
		if (box) {
			height -= 2; 
			width -= 2; 
			start_x += 1;
			start_y += 1;
			outer = win;
			::box(outer, 0, 0);
			wrefresh(outer);
			win = newwin(height, width, start_y, start_x);
			wrefresh(win);
		}
		refresh();
	}

	window::window(int height, int width, int starty, int startx, bool box) : 
		window(newwin(height, width, starty, startx), height, width, starty, startx, box) {}

	window::window(WINDOW *win) : window(win, ::LINES, ::COLS, 0, 0, false) {}

	window::~window() 
	{
		delwin(win);
		if (box)
			delwin(outer);
	}

	void window::set_title(const char *title) 
	{
		int start = width/2 - strlen(title)/2 - 1;
		if (box) {
			::mvwprintw(outer, 0, start, " %s ", title);
			::wrefresh(outer);
		} else { 
			::mvwprintw(win, 0, start, " %s ", title);
			::wrefresh(win);
		}
	}

	void window::clear() 
	{
		::wclear(win);
	}

	void window::addch(chtype ch) 
	{
		::waddch(win, ch);
	}

	void window::addch(charcode code) 
	{
		addch(get_keycode(code));
	}

	void window::move(int y, int x) 
	{
		::wmove(win, y, x);
	}

	chtype window::getch() 
	{
		return ::wgetch(win);
	}

	void window::refresh() 
	{
		::wrefresh(win);
	}

	void window::keypad(bool mode) 
	{
		::keypad(win, mode);
	}

	void window::clrtoeol() 
	{
		::wclrtoeol(win);
	}

	void window::clrtobot() 
	{
		::wclrtobot(win);
	}

	int window::getcurx() 
	{
		return ::getcurx(win);
	}

	int window::getcury() 
	{
		return ::getcury(win);
	}

	void window::printw(const char *fmt, ...) 
	{
		va_list args; 
		va_start(args, fmt);
		::vwprintw(win, fmt, args);
		va_end(args);
		refresh();
	}

	void window::mvprintw(int y, int x, const char *fmt, ...) {
		move(y, x);
		va_list args; 
		va_start(args, fmt);
		::vwprintw(win, fmt, args);
		va_end(args);
		refresh();
	}

	void window::set_scroll(bool val) 
	{
		::scrollok(win, val);
	}

	void window::erase() 
	{
		::werase(win);
	}

	int window::getstr(char *str) 
	{
		return wgetstr(win, str);
	}

	void window::attron(int attr) 
	{
		::wattron(win, attr);
	}

	void window::attroff(int attr) 
	{
		::wattroff(win, attr);
	}
}
