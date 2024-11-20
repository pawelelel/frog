#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <curses.h>

enum Colors
{
	Green = 1,
	Red = 2
};

struct Car
{
	int x, y;
	/*
	 0 => left
	 1 => right
	*/
	int direction;
	int speed;
};

struct InitVariables
{
	int numberOfStreets;
	int width;
	int initPosition;
	int numberOfCars;
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

	Car* cars;
	int carsSize;

	int frames;
	double time;
};

void MoveCars(GameVariables& game)
{
	for (int i = 0; i < game.carsSize; ++i)
	{
		if (game.frames % game.cars[i].speed == 0)
		{
			switch (game.cars[i].direction)
			{
				case 0:
				{
					game.cars[i].x--;
					if (game.cars[i].x < 0)
					{
						game.cars[i].x = game.width - 1;
					}
					break;
				}
				case 1:
				{
					game.cars[i].x++;
					if (game.cars[i].x > game.width - 1)
					{
						game.cars[i].x = 0;
					}
					break;
				}
			}
		}
	}
}

void PrintGameFrame(GameVariables &game)
{
	clear();
	move(1, 0);

	for (int i = 0; i < game.height; ++i)
	{
		for (int j = 0; j < game.width; ++j)
		{
			printw("%c", game.screen[i][j]);
		}
		printw("\n");
	}

	attr_on(COLOR_PAIR(Red), NULL);
	for (int i = 0; i < game.carsSize; ++i)
	{
		move(game.cars[i].y + 1, game.cars[i].x);
		printw("C");
	}
	attr_off(COLOR_PAIR(Red), NULL);

	attr_on( COLOR_PAIR(Green), NULL );
	move(game.frogY + 1, game.frogX);
	printw("F");
	attr_off(COLOR_PAIR(1), NULL);
}

void CheckCollision(GameVariables& game)
{
	for (int i = 0; i < game.carsSize; ++i)
	{
		if (game.cars[i].x == game.frogX && game.cars[i].y == game.frogY)
		{
			game.run = false;
		}
	}
}

void GameFrame(GameVariables &game, CursesVariables & curses)
{
	game.frames++;
	MoveCars(game);
	CheckCollision(game);
	PrintGameFrame(game);
}

bool Init(GameVariables& game, CursesVariables& curses)
{
	srand(time(NULL));
	InitVariables init = {10, 50, 2, 20 };

	// frog init
	game.frogX = init.initPosition;
	game.frogY = init.numberOfStreets * 2;

	// other variables
	game.run = true;
	game.height = init.numberOfStreets * 2 + 1;
	game.width = init.width;
	game.frames = 0;

	// screen
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

	// cars
	game.carsSize = init.numberOfCars;
	game.cars = new Car[game.carsSize];
	for (int i = 0, j = 1; i < game.carsSize; ++i, j += 2)
	{
		game.cars[i].direction = rand() % 2;
		game.cars[i].speed = (rand() % 4 + 1) * 100;
		game.cars[i].x = rand() % game.width;

		int random = rand() % init.numberOfStreets * 2;
		if (random % 2 == 0)
		{
			random++;
		}
		game.cars[i].y = random;
	}

	// curses init
	curses.win = initscr();
	//curs_set(0);
	//keypad(stdscr, TRUE);
	//nonl();
	//raw();
	//noecho();
	//notimeout(stdscr, TRUE);
	//nodelay(curses.win, TRUE);

	// colors init
	curses.colors = has_colors();
	if (curses.colors)
	{
		start_color();
		init_pair(Green, COLOR_GREEN, COLOR_BLACK);
		init_pair(Red, COLOR_RED, COLOR_BLACK);
	}

	return true;
}

void EndProgram(GameVariables& game)
{
	for (int i = 0; i < game.width; ++i)
	{
		//delete[] game.screen[i];
	}

	clear();
	printw("Koniec gry");
	printw("Czas gry: %.2lfs", game.time);
	

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

	clock_t tic = clock();
	while (game.run)
	{
		GameFrame(game, curses);
		HandleInput(game, curses);

		clock_t toc = clock();
		move(0, 0);
		game.time = (double)(toc - tic) / CLOCKS_PER_SEC;
		printw("Czas gry: %.2lfs", game.time);
		wrefresh(curses.win);
	}

	EndProgram(game);

	return 0;
}
