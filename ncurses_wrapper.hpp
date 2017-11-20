#ifndef NCURSES_WRAPPER_H
#define NCURSES_WRAPPER_H

typedef struct _win_st WINDOW;

namespace ncurses {

	class window;
	enum class charcode : unsigned char;

	typedef unsigned int chtype;

	ncurses::window initscr();
	void endwin();
	void set_echo(bool val);
	void set_cursor_visible(bool val);
	void set_cbreak(bool val);
	int get_lines();
	int get_cols();
	chtype get_keycode(charcode c);

	class window {
		WINDOW *win;
		WINDOW *outer;
		bool box;

	protected:
		int height;
		int width;
		int start_y;
		int start_x;

	public:
		window(WINDOW *window, int h, int w, int y, int x, bool box);
		window(int height, int width, int starty, int startx, bool box = false);
		window(WINDOW *win);
		~window();

		void set_title(const char *title);
		void clear();
		void addch(chtype ch);
		void addch(charcode code);
		void move(int y, int x);
		chtype getch();
		void refresh();
		void keypad(bool mode);
		void clrtoeol();
		void clrtobot();
		int getcurx();
		int getcury();
		void printw(const char *fmt, ...);
		void mvprintw(int y, int x, const char *fmt, ...);
		void set_scroll(bool val);
		void erase();
		int getstr(char *str);
		void attron(int attr);
		void attroff(int attr);
	};

	enum key {
		KEY_CODE_YES = 0400,
		KEY_MIN,
		KEY_BREAK = 0401,
		KEY_SRESET = 0530,
		KEY_RESET,
		KEY_DOWN = 0402,
		KEY_UP,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_HOME,
		KEY_BACKSPACE,
		KEY_F0,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_DL = 0510,
		KEY_IL,
		KEY_DC,
		KEY_IC,
		KEY_EIC,
		KEY_CLEAR,
		KEY_EOS,
		KEY_EOL,
		KEY_SF,
		KEY_SR,
		KEY_NPAGE,
		KEY_PPAGE,
		KEY_STAB,
		KEY_CTAB,
		KEY_CATAB,
		KEY_ENTER,
		KEY_PRINT = 0532,
		KEY_LL,
		KEY_A1,
		KEY_A3,
		KEY_B2,
		KEY_C1,
		KEY_C3,
		KEY_BTAB,
		KEY_BEG,
		KEY_CANCEL,
		KEY_CLOSE,
		KEY_COMMAND,
		KEY_COPY,
		KEY_CREATE,
		KEY_END,
		KEY_EXIT,
		KEY_FIND,
		KEY_HELP,
		KEY_MARK,
		KEY_MESSAGE,
		KEY_MOVE,
		KEY_NEXT,
		KEY_OPEN,
		KEY_OPTIONS,
		KEY_PREVIOUS,
		KEY_REDO,
		KEY_REFERENCE,
		KEY_REFRESH,
		KEY_REPLACE,
		KEY_RESTART,
		KEY_RESUME,
		KEY_SAVE,
		KEY_SBEG,
		KEY_SCANCEL,
		KEY_SCOMMAND,
		KEY_SCOPY,
		KEY_SCREATE,
		KEY_SDC,
		KEY_SDL,
		KEY_SELECT,
		KEY_SEND,
		KEY_SEOL,
		KEY_SEXIT,
		KEY_SFIND,
		KEY_SHELP,
		KEY_SHOME,
		KEY_SIC,
		KEY_SLEFT,
		KEY_SMESSAGE,
		KEY_SMOVE,
		KEY_SNEXT,
		KEY_SOPTIONS,
		KEY_SPREVIOUS,
		KEY_SPRINT,
		KEY_SREDO,
		KEY_SREPLACE,
		KEY_SRIGHT,
		KEY_SRSUME,
		KEY_SSAVE,
		KEY_SSUSPEND,
		KEY_SUNDO,
		KEY_SUSPEND,
		KEY_UNDO,
		KEY_MOUSE,
		KEY_RESIZE,
		KEY_EVENT,
		KEY_MAX	= 0777
	};

	enum color {
		COLOR_BLACK,
		COLOR_RED,
		COLOR_GREEN,
		COLOR_YELLOW,
		COLOR_BLUE,
		COLOR_MAGENTA,
		COLOR_CYAN,
		COLOR_WHITE,
	};

	enum class charcode : unsigned char {
		ACS_ULCORNER = 'l',  /* upper left corner */
		ACS_LLCORNER = 'm',	/* lower left corner */
	 	ACS_URCORNER = 'k',	/* upper right corner */
		ACS_LRCORNER = 'j',	/* lower right corner */
		ACS_LTEE = 't',		/* tee pointing right */
		ACS_RTEE = 'u',		/* tee pointing left */
		ACS_BTEE = 'v',		/* tee pointing up */
		ACS_TTEE = 'w',		/* tee pointing down */
		ACS_HLINE = 'q',	/* horizontal line */
		ACS_VLINE = 'x',	/* vertical line */
		ACS_PLUS = 'n',	 	/* large plus or crossover */
		ACS_S1	 = 'o',	 	/* scan line 1 */
		ACS_S9	 = 's',	 	/* scan line 9 */
		ACS_DIAMOND = '`',	/* diamond */
		ACS_CKBOARD = 'a',	/* checker board (stipple,	 */
		ACS_DEGREE = 'f',	/* degree symbol */
		ACS_PLMINUS = 'g',	/* plus/minus */
		ACS_BULLET = '~',	/* bullet */
		ACS_LARROW = ',',	/* arrow pointing left */
		ACS_RARROW = '+',	/* arrow pointing right */
		ACS_DARROW = '.',	/* arrow pointing down */
		ACS_UARROW = '-',	/* arrow pointing up */
		ACS_BOARD = 'h',	/* board of squares */
		ACS_LANTERN = 'i',	/* lantern symbol */
		ACS_BLOCK = '0',	/* solid square block */
		ACS_S3	 = 'p',		/* scan line 3 */
		ACS_S7	 = 'r',	 	/* scan line 7 */
		ACS_LEQUAL = 'y',	/* less/equal */
		ACS_GEQUAL = 'z',	/* greater/equal */
		ACS_PI	 = '{',	 	/* Pi */
		ACS_NEQUAL = '|',	/* not equal */
		ACS_STERLING = '}',	/* UK pound sign */
		ACS_BSSB = ACS_ULCORNER, 
		ACS_SSBB = ACS_LLCORNER,
		ACS_BBSS = ACS_URCORNER,
		ACS_SBBS = ACS_LRCORNER,
		ACS_SBSS = ACS_RTEE,
		ACS_SSSB = ACS_LTEE,
		ACS_SSBS = ACS_BTEE,
		ACS_BSSS = ACS_TTEE,
		ACS_BSBS = ACS_HLINE,
		ACS_SBSB = ACS_VLINE,
		ACS_SSSS = ACS_PLUS,
	};
}

#endif