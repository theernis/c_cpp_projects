#include <ncurses.h>
#include <stdlib.h>


#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)


typedef struct {
	int x;
	int y;
} Pos;

Pos cursor;

typedef struct {
	int size;
	int i;
	struct Stack * stack;
} Stack;


int * minefield;
int tile_size = 1;
int mine_count = 0;
int exploded = 0;
const long seed = 3246170981;
int step = 0;


void init_pairs()
{
	for (int f = 0; f < 8; f++)
	{
		for (int b = 0; b < 8; b++)
		{
			init_pair(1 + (f << 3) + b, f, b);
		}
	}
}

int mine_place(int i)
{
	return ((seed+(++step)) * (LINES/tile_size+i) * (COLS/tile_size+i) >> 8) % ((LINES/tile_size) * (COLS/tile_size));
}

void init_minefield()
{
	mine_count = 0;
	exploded = 0;
	minefield = (int*)malloc(sizeof(int) * (LINES/tile_size) * (COLS/tile_size));
	for (int i = 0; i < (LINES/tile_size) * (COLS/tile_size); i++)
	{
		//1  checked
		//2  flaged
		//4  mine
		//8  1 neighboar mines
		//16 2 neighboar mines
		//32 4 neighboar mines
		//64 8 neighboar mines
		minefield[i] = 0;
	}
	for (int i = 0; i < (LINES+COLS)/tile_size/tile_size; i++)
	{
		while (1)
		{
			int j = mine_place(mine_count++);
			if ((minefield[j] >> 2) % 2 == 1)
			{
				break;
			}
			minefield[j] = 4;
		}
	}
	for (int i = 0; i < (LINES/tile_size) * (COLS/tile_size); i++)
	{
		int sum = 0;
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				if ((i + x + y * (LINES/tile_size)) < 0 || (i + x + y * (LINES/tile_size)) >= (LINES/tile_size) * (COLS/tile_size) || (i % (LINES/tile_size) + x) >= (LINES/tile_size) || (i % (LINES/tile_size) + x) < 0) {continue;}
				sum += (minefield[i + x + y * (LINES/tile_size)] >> 2) % 2;
			}
		}
		minefield[i] += sum << 3;
	}
}

void set_color(int x, int y)
{
	int checked         = (minefield[x + y * (LINES/tile_size)] >> 0) % 2;
	int flaged          = (minefield[x + y * (LINES/tile_size)] >> 1) % 2;
	int mine            = (minefield[x + y * (LINES/tile_size)] >> 2) % 2;
	int neighboar_mines = (minefield[x + y * (LINES/tile_size)] >> 3) % 16;
	wbkgdset(stdscr, COLOR_PAIR(((((neighboar_mines <= 1 || !checked) ? 0 : 8 - ((neighboar_mines + 1) >> 1)) << 3) + ((exploded && flaged && !mine) ? 5 : (((x + y) % 2) ? (2 + (checked)) : (4 - 3 * checked)))) * ((exploded && mine && !flaged) ? 0 : 1) + 1));
	for (int ix = 0; ix < tile_size; ix++)
	{
		for (int iy = 0; iy < tile_size; iy++)
		{
			if (flaged && !(exploded && !mine))
			{
				mvprintw(x*tile_size+ix, y*tile_size+iy, "X");
			}
			else
			{
				if (neighboar_mines > 0 && checked)
				{
					mvprintw(x*tile_size+ix, y*tile_size+iy, "%d\r", MIN(8, neighboar_mines));
				}
				else
				{
					mvprintw(x*tile_size+ix, y*tile_size+iy, " ");
				}
			}
		}
	}
}

void background()
{
	for (int x = 0; x < (LINES/tile_size); x++)
	{
		for (int y = 0; y < (COLS/tile_size); y++)
		{
			set_color(x, y);
		}
	}
}

void try_mine(int x, int y)
{
	if (minefield[x + y * (LINES/tile_size)] & (2 + 1)) {return;}
	minefield[x + y * (LINES/tile_size)] |= 1;
	exploded |= (minefield[x + y * (LINES/tile_size)] >> 2) % 2;
	if ((minefield[x + y * (LINES/tile_size)] >> 3) % 16 != 0) {return;}
	for (int ix = -1; ix <= 1; ix++)
	{
		for (int iy = -1; iy <= 1; iy++)
		{
			if (((x+ix) + (y+iy) * (LINES/tile_size)) < 0 || ((x+ix) + (y+iy) * (LINES/tile_size)) >= (LINES/tile_size) * (COLS/tile_size) || ((x + y * (LINES/tile_size)) % (LINES/tile_size) + ix) >= (LINES/tile_size) || (x + y * (LINES/tile_size) % (LINES/tile_size) + ix) < 0) {continue;}
			try_mine(x+ix, y+iy);
		}
	}
}

int is_solved()
{
	for (int i = 0; i < (LINES/tile_size) * (COLS/tile_size); i++)
	{
		if (!(minefield[i] & 1 || minefield[i] & 4))
		{
			return 0;
		}
	}
	return 1;
}

int main()
{
	cursor.x = 0;
	cursor.y = 0;	
	int ch;

	initscr();
	start_color();

	init_pairs();

	init_minefield();
	background();

	keypad(stdscr, TRUE);
	while (1)
	{
		cursor.x = MAX(0, MIN(LINES/tile_size-1, cursor.x));
		cursor.y = MAX(0, MIN(COLS/tile_size-1, cursor.y));
		set_color(cursor.x, cursor.y);
		move(cursor.x*tile_size, cursor.y*tile_size);
		refresh();

		ch = getch();
		if (ch == 27)
		{
			break;
		}
		if ((ch == KEY_ENTER || ch == '\n') && (exploded || is_solved()))
		{
			//regenerate
			cursor.x = 0;
			cursor.y = 0;
			free(minefield);
			init_minefield();
			background();
			continue;
		}
		if (exploded || is_solved())
		{
			continue;
		}
		if (ch == ' ')
		{
			if (minefield[cursor.x + cursor.y * (LINES/tile_size)] & 1)
			{
				int flags = 0;
				for (int ix = -1; ix <= 1; ix++)
				{
					for (int iy = -1; iy <= 1; iy++)
					{
						if (((cursor.x+ix) + (cursor.y+iy) * (LINES/tile_size)) < 0 || ((cursor.x+ix) + (cursor.y+iy) * (LINES/tile_size)) >= (LINES/tile_size) * (COLS/tile_size) || ((cursor.x + cursor.y * (LINES/tile_size)) % (LINES/tile_size) + ix) >= (LINES/tile_size) || (cursor.x + cursor.y * (LINES/tile_size) % (LINES/tile_size) + ix) < 0) {continue;}
						flags += (minefield[(cursor.x+ix) + (cursor.y+iy) * (LINES/tile_size)] >> 1) % 2;
					}
				}
				if ((minefield[cursor.x + cursor.y * (LINES/tile_size)]) % 16 == flags)
				{
					for (int ix = -1; ix <= 1; ix++)
					{
						for (int iy = -1; iy <= 1; iy++)
						{
							if (((cursor.x+ix) + (cursor.y+iy) * (LINES/tile_size)) < 0 || ((cursor.x+ix) + (cursor.y+iy) * (LINES/tile_size)) >= (LINES/tile_size) * (COLS/tile_size) || ((cursor.x + cursor.y * (LINES/tile_size)) % (LINES/tile_size) + ix) >= (LINES/tile_size) || (cursor.x + cursor.y * (LINES/tile_size) % (LINES/tile_size) + ix) < 0) {continue;}
							try_mine(cursor.x+ix, cursor.y+iy);
						}
					}
				}
			}
			else
			{
				try_mine(cursor.x, cursor.y);
			}
			background();
			continue;
		}
		switch (ch)
		{
			case KEY_UP:
				cursor.x--;
				break;
			case KEY_DOWN:
				cursor.x++;
				break;
			case KEY_RIGHT:
				cursor.y++;
				break;
			case KEY_LEFT:
				cursor.y--;
				break;
			case 'x':
			case 'X':
				if (minefield[cursor.x + cursor.y * (LINES/tile_size)] & 1) {break;}
				minefield[cursor.x + cursor.y * (LINES/tile_size)] ^= 2;
				break;
		}
	}
	endwin();
	return 0;
}
