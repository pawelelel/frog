#include <stdio.h>
#include <conio.h>
#include <string.h>
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

enum Direction
{
	Left = 0,
	Right = 1
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
	void (*done)(GameState&, void*);

	void* data;
};

struct Player
{
	char name[11];
	int points;
};

struct GameOverMessageData
{
	bool won;
	int points;
	Player players[5];
};

struct Car
{
	int size;
	float speed; // # per second
	float x; // in mili #
	int roadNumber;
};

struct Frog
{
	int x, y;
	int skin;
};

struct Road
{
	RoadType type;
	Direction direction;
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
	Car* cars;
	int carsSize;
};

// Window

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
		InitColor(FrogBlood, 120, 6, 6);
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

// Start

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
	StartPair(FrogGreen_Black);
	printw("    JJJ  U  U  M     M  PPP   I  N   N   GG         \n");
	printw("      J  U  U  MM   MM  P  P  I  NN  N  G  G        \n");
	printw("      J  U  U  M M M M  PPP   I  N N N  G           \n");
	printw("      J  U  U  M  M  M  P     I  N  NN  G  GG       \n");
	printw("    JJ    UU   M     M  P     I  N   N   GG         \n");
	printw("                                                    \n");
	printw("             FFF  RRR    OO    GG                   \n");
	printw("             F    R  R  O  O  G  G                  \n");
	printw("             FFF  RRR   O  O  G                     \n");
	printw("             F    R R   O  O  G  GG                 \n");
	printw("             F    R  R   OO    GG                   \n");
	printw("                                                    \n");
	printw("                   (.)_(.)                          \n");
	printw("                _ (  ,_,  ) _                       \n");
	printw("               / \\/`-----'\\/ \\                         \n");
	printw("             __\\ ( (     ) ) /__                    \n");
	printw("             )   /\\ \\._./ /\\   (                    \n");
	printw("              )_/ /|\\   /|\\ \\_(                     \n");
	printw("                                                    \n");
	printw("             s => start new game                    \n");
	printw("             q => quit program                      \n");
	EndPair(FrogGreen_Black);
	wrefresh(win);
}

void StartDone(GameState& self, void* initData)
{
	// uncomment if start has any initdata
	//delete self.data;
	//delete initData;
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

// Game

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
			if (board->frog.y > 0)
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
			if (board->frog.y < board->roadsSize - 1)
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


	// move cars
	int deltaTime = time - b->time;
	for (int i = 0; i < b->carsSize; ++i)
	{
		Car& c = b->cars[i];
		switch (b->roads[b->cars[i].roadNumber].direction)
		{
		case Left:
			{
			c.x -= deltaTime * c.speed / 1000.0f;

			if (c.x <= 0)
			{
				c.x = b->width - c.size + 1;
			}
			break;
			}
		case Right:
			{
			c.x += deltaTime * c.speed / 1000.0f;

			if (c.x >= b->width)
			{
				c.x = 1.0f - c.size;
			}
			break;
			}
		}

		
		
	}

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
	const int UpperStatusAreaSize = 1;
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

	printw("Pawel Leczkowski 203700");

	// cars
	const char carsChars[4][4] = { "", "", "H", "HH" };
	for (int i = 0; i < board->carsSize; ++i)
	{
		Car& c = board->cars[i];
		switch (board->roads[c.roadNumber].direction) {
		case Left:
			{
			move(c.roadNumber + UpperStatusAreaSize, c.x);
			StartPair(Window_Black);
			printw("<");
			EndPair(Window_Black);
			printw(carsChars[c.size]);
			break;
			}
		case Right:
			{
			move(c.roadNumber + UpperStatusAreaSize, c.x);
			printw(carsChars[c.size]);
			StartPair(Window_Black);
			printw(">");
			EndPair(Window_Black);
			break;
			}
		}
	}

	move(board->frog.y + UpperStatusAreaSize, board->frog.x);

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

	move(board->home.y + UpperStatusAreaSize, board->home.x);
	StartPair(Black_Brick);
	printw("H");
	EndPair(Black_Brick);



	wrefresh(win);
}

void GameDone(GameState& self, void* initData)
{
	//delete self.data;
	delete initData;
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

	for (int i = 0; i < board->roadsSize / 2; ++i)
	{
		board->roads[i].direction = Right;
	}

	for (int i = board->roadsSize / 2; i < board->roadsSize; ++i)
	{
		board->roads[i].direction = Left;
	}

	board->frog = { board->width / 2, board->roadsSize - 1, 0 };
	int homeY = rand() % board->width;
	board->home = { homeY, 0 };
	board->score = 200;
	board->time = 0;

	board->carsSize = 5;
	board->cars = new Car[board->carsSize];
	board->cars[0] = { 3, 4.0f, -1, 1};
	board->cars[1] = { 2, 2.5f, -1, 2};
	board->cars[2] = { 1, 2.7f, 0, 5};
	board->cars[3] = { 2, 5.5f, -1, 6};
	board->cars[4] = { 3, 8.0f, -1, 7};

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

// Game over

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

void GameOverDone(GameState& self, void* initData)
{
	delete initData;
}

void GameOverInit(GameState& self, void* initData)
{
	GameOverMessageData* data = (GameOverMessageData*)initData;
	self.data = data;

	FILE* file;
	file = fopen("best.txt", "r");
	if (file == NULL)
	{
		for (int i = 0; i < 5; ++i)
		{
			strcpy(data->players[i].name, "\0");
			data->players[i].points = 0;
		}
	}
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
		current.done(current, current.data);
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
