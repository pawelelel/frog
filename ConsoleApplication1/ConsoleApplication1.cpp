#include <stdio.h>
#include <conio.h>
#include <cstdlib>
#include <curses.h>

struct InitVariables
{
	int numberOfStreets;
	int width;
	int initPosition;
};

struct CursesVariables
{
	bool colors;
	WINDOW *win;
};

struct GameVariables
{
	int width, height;
	char** screen;
	bool run;

	int frogX, frogY;
};

void PrintGameFrame(GameVariables &game)
{
	clear();

	for (int i = 0; i < game.height; ++i)
	{
		for (int j = 0; j < game.width; ++j)
		{
			printw("%c", game.screen[i][j]);
		}
		printw("\n");
	}
	attr_on( COLOR_PAIR(1), NULL );
	move(game.frogY, game.frogX);
	printw("F");
	attr_off(COLOR_PAIR(1), NULL);
}

void GameFrame(GameVariables &game, CursesVariables & curses)
{
	/*for (int i = 0; i < game.height; ++i)
	{
		for (int j = 0; j < game.width; ++j)
		{
			game.screen[i][j] = game.input;
		}
	}*/

	PrintGameFrame(game);
	wrefresh(curses.win);
}

bool Init(GameVariables& game, CursesVariables& curses)
{
	InitVariables init = {5, 10, 2 };

	game.frogX = init.initPosition;
	game.frogY = init.numberOfStreets * 2;


	game.run = true;
	game.height = init.numberOfStreets * 2 + 1;
	game.width = init.width;

	game.screen = new char*[game.height];

	for (int i = 0; i < game.height; ++i)
	{
		game.screen[i] = new char[game.width];
		for (int j = 0; j < game.width; ++j)
		{
			if (i % 2)
			{
				game.screen[i][j] = '=';
			}
			else
			{
				game.screen[i][j] = '+';
			}
		}
	}

	// curses init
	curses.win = initscr();
	curs_set(0);
	keypad(stdscr, TRUE);
	nonl();
	raw();
	noecho();
	notimeout(stdscr, TRUE);
	nodelay(curses.win, TRUE);

	// colors init
	curses.colors = has_colors();
	if (curses.colors)
	{
		start_color();
		init_pair(1, COLOR_GREEN, COLOR_BLACK);
	}

	return true;
}

void EndProgram(GameVariables& game)
{
	for (int i = 0; i < game.width; ++i)
	{
		delete[] game.screen[i];
	}
	endwin();
}

void HandleInput(GameVariables& game, CursesVariables& curses)
{
	if (_kbhit())
	{
		int input = getchar();

		switch (input)
		{
			case 'w':
			{
				if (game.frogY > 0)
				{
					game.frogY--;
				}
				break;
			}
			case 'a':
			{
				if (game.frogX > 0)
				{
					game.frogX--;
				}
				break;
			}
			case 'd':
			{
				if (game.frogX < game.width - 1)
				{
					game.frogX++;
				}
				break;
			}
			case 'q':
			{
				game.run = false;
				break;
			}
		}
	}
}

int main()
{
	GameVariables game = {};
	CursesVariables curses = {};

	if (!Init(game, curses))
	{
		EndProgram(game);
		return -1;
	}
	

	while (game.run)
	{
		GameFrame(game, curses);
		HandleInput(game, curses);
	}

	EndProgram(game);

	return 0;
}
