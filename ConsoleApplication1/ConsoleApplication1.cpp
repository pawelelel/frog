#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <curses.h>

// enums

enum ColorPairs
{
	GrassGreen_GrassGreen = 1,
	FrogGreen_Black = 2,
	RoadGray_RoadGray = 3,
	Black_Brick = 4,
	Yellow_Black = 5,
	FrogBlood_Black = 6,
	FrogBlood_FrogBlood = 7,
	Window_Black = 8,
	Brick_Black = 9,
	GrassGreen_Black = 10,
};

enum Colors
{
	Black = COLOR_BLACK,
	Blue = COLOR_BLUE,
	Green = COLOR_GREEN,
	Cyan = COLOR_CYAN,
	Red = COLOR_RED,
	Magenta = COLOR_MAGENTA,
	Yellow = COLOR_YELLOW,
	White = COLOR_WHITE,
	GrassGreen = 8,
	FrogGreen = 9,
	RoadGray = 10,
	Brick = 11,
	FrogBlood = 12,
	Window = 13
};

enum State
{
	Start = 0,
	Game = 1,
	GameOver = 2
};

enum GameStateMessage
{
	ChangeNoChange = 0,
	ChangeToStart = 1,
	ChangeToGame = 2,
	ChangeToGameOver = 3,
	ExitProgram = 4
};

enum RoadType
{
	Grass = 0,
	Street = 1
};

// structs

struct GameStateChange
{
	GameStateMessage message;
	void* data;
};

struct GameState
{
	void (*init)(GameState&, void*);
	GameStateChange(*keysHandler)(GameState&, int);
	GameStateChange(*timerHandler)(GameState&, int);
	void (*draw)(GameState&, WINDOW*);
	void (*done)();

	void* data;
};

struct GameOverMessageData
{
	bool won;
	int points;
};

struct Frog
{
	int x, y;
	int skin;
};

struct Road
{
	RoadType type;
};

struct Home
{
	int x, y;
};

struct Board
{
	Frog frog;
	Road* roads;
	Home home;
	int roadsSize;
	int width;
	int score;
	int time;
};

// window

void InitColor(int colorId, short r, short g, short b)
{
	r = r * 200 / 51;
	g = g * 200 / 51;
	b = b * 200 / 51;

	init_color(colorId, r, g, b);
}

void InitColorPair(short pairId, short colorFont, short colorBack)
{
	init_pair(pairId, colorFont, colorBack);
}

void StartPair(int pairId)
{
	attr_on(COLOR_PAIR(pairId), NULL);
}

void EndPair(int pairId)
{
	attr_off(COLOR_PAIR(pairId), NULL);
}

WINDOW* InitWindow()
{
	// curses init
	WINDOW*win = initscr();
	int col = 56, row = 45;
	resize_term(row, col);
	curs_set(0);

	// colors init
	if (has_colors())
	{
		start_color();

		InitColor(GrassGreen, 65, 152, 10);
		InitColor(FrogGreen, 153, 198, 142);
		InitColor(RoadGray, 179, 179, 179);
		InitColor(Brick, 192, 50, 72);
		InitColor(FrogBlood, 19, 109, 21);
		InitColor(Window, 0, 153, 255);

		InitColorPair(GrassGreen_GrassGreen, GrassGreen, GrassGreen);
		InitColorPair(FrogGreen_Black, FrogGreen, Black);
		InitColorPair(RoadGray_RoadGray, RoadGray, RoadGray);
		InitColorPair(Black_Brick, Black, Brick);
		InitColorPair(Yellow_Black, Yellow, Black);
		InitColorPair(FrogBlood_Black, FrogBlood, Black);
		InitColorPair(FrogBlood_FrogBlood, FrogBlood, FrogBlood);
		InitColorPair(Window_Black, Window, Black);
		InitColorPair(Brick_Black, Brick, Black);
		InitColorPair(GrassGreen_Black, GrassGreen, Black);
	}

	return win;
}

// Start struct init

GameStateChange StartKeysHandler(GameState& self, int key)
{
	switch (key)
	{
		case 'q':
		{
			return {ExitProgram, NULL};
		}
		case 's':
		{
			return { ChangeToGame, NULL };
		}
		default:
		{
			return { ChangeNoChange, NULL };
		}
	}
}

GameStateChange StartTimerHandler(GameState& self, int time)
{
	return { ChangeNoChange, NULL };
}

void StartDraw(GameState& self, WINDOW* win)
{
	clear();
	printw("Start\n");
	printw("s => start new game\n");
	printw("q => quit program\n");
	wrefresh(win);
}

void StartDone()
{
	
}

void StartInit(GameState& init, void* initData)
{
	
}

GameState CreateStart()
{
	GameState start;
	start.init = &StartInit;
	start.keysHandler = &StartKeysHandler;
	start.timerHandler = &StartTimerHandler;
	start.draw = &StartDraw;
	start.done = &StartDone;
	return start;
}

// Game struct init

bool IsFrogInHome(Frog frog, Home home)
{
	if (frog.x == home.x && frog.y == home.y)
	{
		return true;
	}
	return false;
}

GameStateChange GameKeysHandler(GameState& self, int key)
{
	Board* board = (Board*)self.data;

	switch (key)
	{
		case 'q':
		{
			return { ChangeToStart, NULL };
		}
		case 'w':
		{
			if (board->frog.y > 1)
			{
				board->frog.y--;
				if (IsFrogInHome(board->frog, board->home))
				{
					GameOverMessageData* data = new GameOverMessageData{ true, board->score };
					return { ChangeToGameOver, data};
				}
			}
			return { ChangeNoChange, NULL };
		}
		case 'a':
		{
			if (board->frog.x > 0)
			{
				board->frog.x--;
				if (IsFrogInHome(board->frog, board->home))
				{
					GameOverMessageData* data = new GameOverMessageData{ true, board->score };
					return { ChangeToGameOver, data };
				}
			}
			return { ChangeNoChange, NULL };
		}
		case 's':
		{
			if (board->frog.y < board->roadsSize)
			{
				board->frog.y++;
				if (IsFrogInHome(board->frog, board->home))
				{
					GameOverMessageData* data = new GameOverMessageData{ true, board->score };
					return { ChangeToGameOver, data };
				}
			}
			return { ChangeNoChange, NULL };
		}
		case 'd':
		{
			if (board->frog.x < board->width - 1)
			{
				board->frog.x++;
				if (IsFrogInHome(board->frog, board->home))
				{
					GameOverMessageData* data = new GameOverMessageData{ true, board->score };
					return { ChangeToGameOver, data };
				}
			}
			return { ChangeNoChange, NULL };
		}
		default:
		{
			return { ChangeNoChange, NULL };
		}
	}
}

GameStateChange GameTimerHandler(GameState& self, int time)
{
	Board* b = (Board*)self.data;
	b->frog.skin = time / 1000 % 2;
	b->time = time;

	if (time > 10000)
	{
		GameOverMessageData* data = new GameOverMessageData{ false, 0 };
		return { ChangeToGameOver, data };
	}
	return { ChangeNoChange, NULL };
}

void DrawStreet(int width)
{
	StartPair(RoadGray_RoadGray);
	for (int i = 0; i < width; ++i)
	{
		printw("=");
	}
	EndPair(RoadGray_RoadGray);
}

void DrawGrass(int width)
{
	StartPair(GrassGreen_GrassGreen);
	for (int i = 0; i < width; ++i)
	{
		printw(" ");
	}
	EndPair(GrassGreen_GrassGreen);
}

void GameDraw(GameState& self, WINDOW*win)
{
	clear();

	Board* board = (Board*)self.data;

	printw("Time: %ds    Score: %d\n", board->time/1000, board->score);

	// roads
	for (int i = 0; i < board->roadsSize; ++i)
	{
		switch (board->roads[i].type) {
			case Grass:
			{
				DrawGrass(board->width);
				break;
			}
			case Street:
			{
				DrawStreet(board->width);
				break;
			}
		}
		printw("\n");
	}

	printw("Pawe� Leczkowski 203700");

	move(board->frog.y, board->frog.x);

	StartPair(FrogGreen_Black);
	if (board->frog.skin == 0)
	{
		printw("F");
	}
	else
	{
		printw("f");
	}
	EndPair(FrogGreen_Black);

	move(board->home.y, board->home.x);
	StartPair(Black_Brick);
	printw("H");
	EndPair(Black_Brick);



	wrefresh(win);
}

void GameDone()
{
	
}

void GameInit(GameState& self, void* initData)
{
	Board* board = new Board;
	board->width = 50;
	board->roadsSize = 10;
	board->roads = new Road[board->roadsSize];
	board->roads[0].type = Grass;
	board->roads[1].type = Street;
	board->roads[2].type = Street;
	board->roads[3].type = Grass;
	board->roads[4].type = Grass;
	board->roads[5].type = Street;
	board->roads[6].type = Street;
	board->roads[7].type = Street;
	board->roads[8].type = Grass;
	board->roads[9].type = Grass;
	board->frog = { board->width / 2, board->roadsSize, 0 };
	int homeY = rand() % board->width;
	board->home = { homeY, 1 };
	board->score = 200;
	board->time = 0;

	self.data = board;
}

GameState CreateGame()
{
	GameState game;
	game.init = &GameInit;
	game.keysHandler = &GameKeysHandler;
	game.timerHandler = &GameTimerHandler;
	game.draw = &GameDraw;
	game.done = &GameDone;
	return game;
}

// Game over struct init

GameStateChange GameOverKeysHandler(GameState& self, int key)
{
	switch (key)
	{
		case 'q':
		{
			return { ChangeToStart, NULL };
		}
		default:
		{
			return { ChangeNoChange, NULL };
		}
	}
}

GameStateChange GameOverTimerHandler(GameState& self, int time)
{
	return { ChangeNoChange, NULL };
}

void GameOverDraw(GameState& self, WINDOW*win)
{
	GameOverMessageData* data = (GameOverMessageData*)self.data;

	clear();
	if (data->won)
	{
		printw("\n");
		printw("        Y   Y   OO   U  U    W   W   OO   N   N        \n");
		printw("         Y Y   O  O  U  U    W   W  O  O  NN  N        \n");
		printw("          Y    O  O  U  U    W   W  O  O  N N N        \n");
		printw("          Y    O  O  U  U    W W W  O  O  N  NN        \n");
		printw("          Y     OO    UU      W W    OO   N   N        \n");
		printw("\n"); StartPair(Brick_Black);
		printw("                                    +--+               \n");
		printw("           _________________________|##|_____          \n");
		printw("          /#########################|##|#####\\        \n");
		printw("         /##########################+--+######\\       \n");
		printw("        /######################################\\      \n");
		printw("       /########################################\\     \n");
		printw("      /+----------------------------------------+\\    \n");
		printw("     / |                                        | \\   \n");
		printw("       |                                        |      \n");
		printw("       |  +------+                              |      \n"); EndPair(Brick_Black);
		StartPair(Brick_Black); printw("       |  |      |             "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw("(.)_(.)"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("          |      \n"); EndPair(Brick_Black);
		StartPair(Brick_Black); printw("       |  |      |          "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw("_ (  ,_,  ) _"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("       |      \n"); EndPair(Brick_Black);
		StartPair(Brick_Black); printw("       |  |    o |         "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw("/ \\/`-----'\\/ \\"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("      |      \n"); EndPair(Brick_Black);
		StartPair(Brick_Black); printw("       |  |      |       "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw("__\\ ( (     ) ) /__"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("    |      \n"); EndPair(Brick_Black);
		StartPair(Brick_Black); printw("       |  |      |       "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw(")   /\\ \\._./ /\\   ("); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("    |     \n"); EndPair(Brick_Black);
		StartPair(Brick_Black); printw("       |  |      |        "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw(")_/ /|\\   /|\\ \\_("); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("     |      \n"); EndPair(Brick_Black);
		StartPair(GrassGreen_Black); printw("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); EndPair(GrassGreen_Black);
		printw("                    Your score: %d                     \n", data->points);
		printw("\n");
		printw("                      Enter name:                      \n");
		printw("                       Nam_                            \n");
		printw("                      Best scores:                     \n");
		printw("                  +------------+----+                  \n");
		printw("                  |     You    | 65 |                  \n");
		printw("                  +------------+----+                  \n");
		printw("                  | Name 2 cos | 50 |                  \n");
		printw("                  +------------+----+                  \n");
		printw("                  | Name 3 cos | 30 |                  \n");
		printw("                  +------------+----+                  \n");
		printw("                  | Name 4 cos | 20 |                  \n");
		printw("                  +------------+----+                  \n");
		printw("                  | Name 5 cos | 20 |                  \n");
		printw("                  +------------+----+                  \n");
		printw("\n");
		printw("                 q => quit to main menu                \n");
	}
	else
	{
		printw("\n");
		printw("      Y   Y   OO   U  U    L     OO    SS   TTTTT      \n");
		printw("       Y Y   O  O  U  U    L    O  O  S       T        \n");
		printw("        Y    O  O  U  U    L    O  O   SS     T        \n");
		printw("        Y    O  O  U  U    L    O  O     S    T        \n");
		printw("        Y     OO    UU     LLL   OO    SS     T        \n");
		printw("\n");
		printw("                                  +--------------------\n");
		printw("                                 //---------++---------\n");
		printw("                                /"); StartPair(Window_Black); printw("/##########"); EndPair(Window_Black); printw("||"); StartPair(Window_Black); printw("#########"); EndPair(Window_Black); printw("\n");
		printw("                               /"); StartPair(Window_Black); printw("/###########"); EndPair(Window_Black); printw("||"); StartPair(Window_Black); printw("#########"); EndPair(Window_Black); printw("\n");
		printw("                              /"); StartPair(Window_Black); printw("/############"); EndPair(Window_Black); printw("||"); StartPair(Window_Black); printw("#########"); EndPair(Window_Black); printw("\n");
		printw("                             //-------------++---------\n");
		StartPair(Yellow_Black); printw("\\"); EndPair(Yellow_Black); printw("+--------------------------++--------------++---------\n");
		StartPair(Yellow_Black); printw("-###"); EndPair(Yellow_Black); printw("                         |           O  ||         \n");
		StartPair(Yellow_Black); printw("-###"); EndPair(Yellow_Black); printw("       ______            |              ||         \n");
		StartPair(Yellow_Black); printw("/"); EndPair(Yellow_Black); printw("|       / ,-~-, \\           |              ||         \n");
		StartPair(FrogBlood_Black); printw(" |"); EndPair(FrogBlood_Black);  StartPair(FrogBlood_FrogBlood); printw("  "); EndPair(FrogBlood_FrogBlood);  printw("    // \\   / \\\\          |              ||         \n");
		StartPair(FrogBlood_Black); printw(" |"); EndPair(FrogBlood_Black); StartPair(FrogBlood_FrogBlood); printw("   "); EndPair(FrogBlood_FrogBlood); printw("  |,   \\ /   ,|         |              ||         \n");
		StartPair(FrogBlood_Black); printw(" +-----+|--"); EndPair(FrogBlood_Black); printw("--O----|+-----------------------------------\n");
		StartPair(FrogBlood_Black); printw("        \\"); EndPair(FrogBlood_Black); printw("   / \\   /                                    \n");
		StartPair(FrogBlood_Black); printw("         ',"); EndPair(FrogBlood_Black); printw("/   \\,'                                     \n");
		StartPair(FrogBlood_Black); printw("           '-~-'"); EndPair(FrogBlood_Black); printw("                                       \n");
		StartPair(FrogBlood_FrogBlood); printw("-------------------------"); EndPair(FrogBlood_FrogBlood); StartPair(RoadGray_RoadGray); printw("------------------------------\n"); EndPair(RoadGray_RoadGray);
		printw("                 q => quit to main menu                \n");
	}
	wrefresh(win);
}

void GameOverDone()
{

}

void GameOverInit(GameState& self, void* initData)
{
	GameOverMessageData* data = (GameOverMessageData*)initData;
	self.data = data;
}

GameState CreateGameOver()
{
	GameState gameOver;
	gameOver.init = &GameOverInit;
	gameOver.keysHandler = &GameOverKeysHandler;
	gameOver.timerHandler = &GameOverTimerHandler;
	gameOver.draw = &GameOverDraw;
	gameOver.done = &GameOverDone;
	return gameOver;
}

int ReadKeys()
{
	if (_kbhit())
	{
		int input = getchar();
		return input;
	}
	return NULL;
}

GameStateChange MainLoop(GameState& current, WINDOW* win)
{
	clock_t startTime = clock();
	while (true)
	{
		GameStateChange change;

		int key = ReadKeys();
		if (key != NULL)
		{
			change = current.keysHandler(current, key);
			if (change.message != ChangeNoChange)
			{
				return change;
			}
		}

		clock_t now = clock();
		int time = (now - startTime) * 1000 / CLOCKS_PER_SEC;
		change = current.timerHandler(current, time);
		if (change.message != ChangeNoChange)
		{
			return change;
		}

		current.draw(current, win);
	}
}

int main()
{
	srand(time(NULL));

	GameState Start = CreateStart();
	GameState Game = CreateGame();
	GameState GameOver = CreateGameOver();

	GameState current = Start;
	current.init(current, NULL);

	WINDOW* win = InitWindow();
	
	while (true)
	{
		GameStateChange change = MainLoop(current, win);
		current.done();
		switch (change.message)
		{
			case ChangeToStart:
			{
				current = Start;
				break;
			}
			case ChangeToGame:
			{
				current = Game;
				break;
			}
			case ChangeToGameOver:
			{
				current = GameOver;
				break;
			}
			case ExitProgram:
			{
				return 0;
			}
		}
		current.init(current, change.data);
	}
}

/*
ascii art zaby ladnie na poczatku
ladniejsze wyswietlanie (szare ulice (jak trawa) i kolory z rgb) domek czerwony
rozjechana zaba i zaba w domu ascii w game over
punktacja w game over wyswietlanie bez logiki
pasek na gorze z czasem i punktami w czasie gry
pasek na dole z imie nazwisko index caly czas
*/

/*
struct Car
{
	int x, y;

	int direction;
	int speed;
};

struct InitVariables
{
	int numberOfStreets;
	int width;
	int initPosition;
	int numberOfCars;
	int seed;
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

	int destinationX, destinationY;
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

	attr_on(COLOR_PAIR(Yellow), NULL);
	for (int i = 0; i < game.carsSize; ++i)
	{
		move(game.cars[i].y + 1, game.cars[i].x);
		printw("C");
	}
	attr_off(COLOR_PAIR(Yellow), NULL);

	attr_on( COLOR_PAIR(Green), NULL );
	move(game.frogY + 1, game.frogX);
	printw("F");
	attr_off(COLOR_PAIR(Green), NULL);

	attr_on(COLOR_PAIR(Red), NULL);
	move(game.destinationY + 1, game.destinationX);
	printw("H");
	attr_off(COLOR_PAIR(Red), NULL);
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

void AmIHome(GameVariables& game)
{
	if (game.destinationX == game.frogX && game.destinationY == game.frogY)
	{
		game.run = false;
	}
}

void GameFrame(GameVariables &game, CursesVariables & curses)
{
	game.frames++;
	MoveCars(game);
	CheckCollision(game);
	AmIHome(game);
	PrintGameFrame(game);
}

bool GetInit(InitVariables&init, const char * filename)
{
	FILE* file;
	file = fopen(filename, "r");

	if (file == NULL)
	{
		return false;
	}

	fscanf(file, "Number of streets: %d\n", &init.numberOfStreets);
	fscanf(file, "Width: %d\n", &init.width);
	fscanf(file, "Number of cars: %d\n", &init.numberOfCars);
	fscanf(file, "Seed: %d\n", &init.seed);
	return true;
}

bool Init(GameVariables& game, CursesVariables& curses)
{
	InitVariables init;
	if (!GetInit(init, "game.config"))
	{
		return false;
	}
	if (init.seed == -1)
	{
		srand(time(NULL));
	}
	else
	{
		srand(init.seed);
	}

	// frog init
	game.frogX = rand() % init.width;
	game.frogY = init.numberOfStreets * 2;

	// home init
	game.destinationX = rand() % init.width;
	game.destinationY = 0;

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
	curs_set(0);

	// colors init
	curses.colors = has_colors();
	if (curses.colors)
	{
		start_color();
		init_pair(Green, COLOR_GREEN, COLOR_BLACK);
		init_pair(Red, COLOR_RED, COLOR_BLACK);
		init_pair(Yellow, COLOR_YELLOW, COLOR_BLACK);
	}

	return true;
}

void EndProgram(GameVariables& game, CursesVariables&curses)
{
	for (int i = 0; i < game.width; ++i)
	{
		//delete[] game.screen[i];
	}

	clear();
	printw("Koniec gry");
	printw("Czas gry: %.2lfs", game.time);
	wrefresh(curses.win);

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
*/

/*
	GameVariables game = {};
	CursesVariables curses = {};

	if (!Init(game, curses))
	{
		EndProgram(game, curses);
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

	EndProgram(game, curses);
	*/