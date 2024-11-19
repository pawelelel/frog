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
	char input;

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


	//attr_on( COLOR_PAIR(1), NULL );
	//attr_off(COLOR_PAIR(1), NULL);
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

	//game.frogX =  


	game.run = true;
	game.height = init.numberOfStreets * 2 + 1;
	game.width = init.width;
	//game.input = 'a';

	game.screen = new char*[game.width];

	for (int i = 0; i < game.height; ++i)
	{
		game.screen[i] = new char[game.height];
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
	delete game.screen;
	endwin();
}

void HandleInput(GameVariables& game, CursesVariables& curses)
{
	if (_kbhit())
	{
		game.input = getchar();
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
		HandleInput(game, curses);
		GameFrame(game, curses);
	}

	EndProgram(game);

	return 0;
}
