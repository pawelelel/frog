// ReSharper disable CppCStyleCast
// ReSharper disable CppClangTidyCertErr33C
// ReSharper disable CppDeprecatedEntity
// ReSharper disable CppClangTidyClangDiagnosticDeprecatedDeclarations
// ReSharper disable CppClangTidyBugproneNarrowingConversions
// ReSharper disable CppClangTidyClangDiagnosticImplicitIntFloatConversion
// ReSharper disable CppClangTidyConcurrencyMtUnsafe
// ReSharper disable CppClangTidyPerformanceTypePromotionInMathFn
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
// ReSharper disable CppClangTidyModernizeDeprecatedHeaders
// ReSharper disable CppFunctionalStyleCast
// ReSharper disable CppClangTidyModernizeLoopConvert
// ReSharper disable CppClangTidyCertErr34C
#include <conio.h>
#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
	Red_Black = 10,
	Green_Black = 11,
	Red_White = 12,
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

enum Speed
{
	Normal = 0,
	Slow = 1,
	Fast = 2
};

enum CarType
{
	Friendly = 0,
	Bad = 1,
	Taxi = 2
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
	GameStateChange(*keysHandler)(const GameState&, int);
	GameStateChange(*timerHandler)(const GameState&, int);
	void (*draw)(const GameState&, WINDOW*);
	void (*done)(GameState&, void*);

	void* data;
};

struct Player
{
	char name[11];
	int score;
};

struct GameOverMessageData
{
	bool won;
	int score;
	Player players[5];

	// for keyboard input
	char str[11];
	bool enter;
	int index;
};

struct Car
{
	int size;
	float speed; // # per second
	float x; // in mili #
	int roadNumber;
	Speed speedType;
	CarType type;
};

struct Frog
{
	int x, y;
	int skin;

	Car* car;
	bool onCar;
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

struct Stork
{
	int x, y;
	float distance, speed;// works same as cars speed
};

struct Building
{
	int x, roadNumber;
};

struct Board
{
	Frog frog;
	Road* roads;
	Home home;
	int roadsSize;
	int width;
	int score;
	int time; // in miliseconds
	int maxTime;// in seconds
	Car* cars;
	int carsSize;
	Building* buildings;
	int buildingsSize;
	Stork stork;
};

// Window

void InitColor(short colorId, int r, int g, int b)
{
	r = r * 200 / 51;
	g = g * 200 / 51;
	b = b * 200 / 51;

	init_color(colorId, (short)r, (short)g, (short)b);
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
		InitColorPair(Red_Black, Red, Black);
		InitColorPair(Green_Black, Green, Black);
		InitColorPair(Red_White, Red, White);
	}

	return win;
}

void Resize(int cols, int lines)
{
	resize_term(1000, 1000);
	resize_term(lines, cols);
}

// Start

GameStateChange StartKeysHandler(const GameState& self, int key)
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

GameStateChange StartTimerHandler(const GameState& self, int time)
{
	return { ChangeNoChange, NULL };
}

void DrawStartFrog()
{
	printw("                     (.)_(.)                        \n");
	printw("                  _ (  ,_,  ) _                     \n");
	printw("                 / \\/`-----'\\/ \\                    \n");
	printw("               __\\ ( (     ) ) /__                  \n");
	printw("               )   /\\ \\._./ /\\   (                  \n");
	printw("                )_/ /|\\   /|\\ \\_(                   \n");
}

void StartDraw(const GameState& self, WINDOW* win)
{
	clear();
	StartPair(FrogGreen_Black);
	printw("     JJJ  U  U  M     M  PPP   I  N   N   GG        \n");
	printw("       J  U  U  MM   MM  P  P  I  NN  N  G  G       \n");
	printw("       J  U  U  M M M M  PPP   I  N N N  G          \n");
	printw("       J  U  U  M  M  M  P     I  N  NN  G  GG      \n");
	printw("     JJ    UU   M     M  P     I  N   N   GG        \n");
	printw("\n");
	printw("              FFF  RRR    OO    GG                  \n");
	printw("              F    R  R  O  O  G  G                 \n");
	printw("              FFF  RRR   O  O  G                    \n");
	printw("              F    R R   O  O  G  GG                \n");
	printw("              F    R  R   OO    GG                  \n");
	printw("\n");
	DrawStartFrog();
	printw("\n");
	printw("               s => start new game                  \n");
	printw("               q => quit program                    \n");
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
	int col = 53, line = 23;
	Resize(col, line);
}

GameState CreateStart()
{
	GameState start;
	start.init = &StartInit;
	start.keysHandler = &StartKeysHandler;
	start.timerHandler = &StartTimerHandler;
	start.draw = &StartDraw;
	start.done = &StartDone;
	start.data = NULL;
	return start;
}

// Game

bool IsFrogInHome(const Frog& frog, const Home& home)
{
	if (frog.x == home.x && frog.y == home.y)
	{
		return true;
	}
	return false;
}

bool CanFrogJump(const int newX, const int newY, const Board* b)
{
	if (newY < 0)
	{
		return false;
	}
	if (newX < 0)
	{
		return false;
	}
	if (newY > b->roadsSize - 1)
	{
		return false;
	}
	if (newX > b->width - 1)
	{
		return false;
	}

	for (int i = 0; i < b->buildingsSize; ++i)
	{
		if (newX == b->buildings[i].x && newY == b->buildings[i].roadNumber)
		{
			return false;
		}
	}
	return true;
}

GameStateChange GameKeysHandler(const GameState& self, int key)
{
	Board* board = (Board*)self.data;
	bool jump = false;

	switch (key)
	{
		case 'q':
		{
			return { ChangeToStart, NULL };
		}
		case 'w':
		{
			jump = CanFrogJump(board->frog.x, board->frog.y - 1, board);
			if (jump)
			{
				board->frog.onCar = false;
				board->frog.car = NULL;
				board->frog.y--;

				if (board->roads[board->frog.y].type == Street)
				{
					board->score++;
				}
			}
			break;
		}
		case 'a':
		{
			jump = CanFrogJump(board->frog.x - 1, board->frog.y, board);
			if (jump)
			{
				board->frog.x--;
			}
			break;
		}
		case 's':
		{
			jump = CanFrogJump(board->frog.x, board->frog.y + 1, board);
			if (jump)
			{
				board->frog.onCar = false;
				board->frog.car = NULL;
				board->frog.y++;

				if (board->roads[board->frog.y].type == Street)
				{
					board->score++;
				}
			}
			break;
		}
		case 'd':
		{
			jump = CanFrogJump(board->frog.x + 1, board->frog.y, board);
			if (jump)
			{
				board->frog.x++;
			}
			break;
		}
	default: break;
	}

	if (jump && IsFrogInHome(board->frog, board->home))
	{
		board->score += board->maxTime - board->time / 1000;
		GameOverMessageData* data = new GameOverMessageData{ true, board->score };
		return { ChangeToGameOver, data };
	}
	return { ChangeNoChange, NULL };
}

bool IsFrogHitted(const Frog& f, const Car& c)
{
	if (c.type == Bad && !f.onCar && f.x == (int)round(c.x) && f.y == c.roadNumber)
	{
		return true;
	}
	return false;
}

void ChangeCarSpeed(Car& c)
{
	// speed up cars
	int speedChange = rand() % 100;
	float speedUpFactor = 5.0f;

	if (c.speedType == Normal)
	{ // change car speed
		if (speedChange < 10) // 10% of chances
		{ // speed up
			c.speed *= speedUpFactor;
			c.speedType = Fast;
		}
		else if (speedChange < 20) // 10% of chances
		{ // slow down
			c.speed *= 1.0f / speedUpFactor;
			c.speedType = Slow;
		}
	}
	else if (speedChange < 1) // 1% of chances
	{ // change it back
		if (c.speedType == Slow)
		{ // speed up
			c.speed *= speedUpFactor;
		}
		else if (c.speedType == Fast)
		{ // slow down
			c.speed *= 1.0f / speedUpFactor;
		}
		c.speedType = Normal;
	}
}

void FrogGetInTaxi(Board* b, Car& c)
{
	b->frog.car = &c;
	if (!b->frog.onCar)
	{
		b->score++;
	}
	b->frog.onCar = true;
}

void ChangeCarRoad(const Board* b, Car& c)
{
	int numberOfStreets = 0;
	for (int i = 0; i < b->roadsSize; ++i)
	{
		if (b->roads[i].type == Street)
		{
			numberOfStreets++;
		}
	}
	int* streets = new int[numberOfStreets];
	int index = 0;
	for (int i = 0; i < b->roadsSize; ++i)
	{
		if (b->roads[i].type == Street)
		{
			streets[index] = i;
			index++;
		}
	}

	c.x = 1.0f - c.size;
	int newRoad = rand() % numberOfStreets;
	c.roadNumber = streets[newRoad];

	delete[] streets;
}

GameStateChange MoveCars(Board* b, int deltaTime)
{
	for (int i = 0; i < b->carsSize; ++i)
	{
		Car& c = b->cars[i];
		switch (b->roads[b->cars[i].roadNumber].direction)
		{
			case Left:
			{
				int distance = (int)round(c.x) - b->frog.x;
				if (c.type == Friendly && 0 <= distance && distance <= 2 && c.roadNumber == b->frog.y)
				{
					break;
				}

				if (c.type == Taxi && (int)round(c.x) - 1 == b->frog.x && c.roadNumber == b->frog.y)
				{
					FrogGetInTaxi(b, c);
				}

				c.x -= deltaTime * c.speed / 1000.0f;

				if (c.x <= 0)
				{
					bool wrap = rand() % 2;

					if (wrap)
					{
						c.x = b->width - c.size + 1;
					}
					else
					{
						ChangeCarRoad(b, c);
					}
				}

				break;
			}
			case Right:
			{
				int distance = b->frog.x - (int)round(c.x);
				if (c.type == Friendly && 0 <= distance && distance <= 2 && c.roadNumber == b->frog.y)
				{
					break;
				}

				if (c.type == Taxi && (int)round(c.x) + 1 == b->frog.x && c.roadNumber == b->frog.y)
				{
					FrogGetInTaxi(b, c);
				}

				c.x += deltaTime * c.speed / 1000.0f;

				if (c.x >= b->width)
				{
					bool wrap = rand() % 2;

					if (wrap)
					{
						c.x = 1.0f - c.size;
					}
					else
					{
						ChangeCarRoad(b, c);
					}
				}
				break;
			}
		}

		ChangeCarSpeed(c);

		if (IsFrogHitted(b->frog, c))
		{
			GameOverMessageData* data = new GameOverMessageData{ false, 0 };
			return { ChangeToGameOver, data };
		}
	}

	return { ChangeNoChange, NULL };
}

GameStateChange StorkFly(Board* b, int deltaTime)
{
	b->stork.distance += deltaTime * b->stork.speed / 1000.0f;
	if (b->stork.distance > 1)
	{
		b->stork.distance--;

		if (b->stork.x < b->frog.x)
		{
			b->stork.x++;
		}
		else if (b->stork.x > b->frog.x)
		{
			b->stork.x--;
		}
		
		if (b->stork.y < b->frog.y)
		{
			b->stork.y++;
		}
		else if (b->stork.y > b->frog.y)
		{
			b->stork.y--;
		}

		if (b->stork.x == b->frog.x && b->stork.y == b->frog.y)
		{
			GameOverMessageData* data = new GameOverMessageData{ false, 0 };
			return { ChangeToGameOver, data };
		}
	}
	return { ChangeNoChange, NULL };
}

GameStateChange GameTimerHandler(const GameState& self, int time)
{
	Board* b = (Board*)self.data;
	b->frog.skin = time / 1000 % 2;

	if (b->frog.onCar)
	{
		b->frog.x = (int)round(b->frog.car->x);
		b->frog.y = b->frog.car->roadNumber;
	}

	// move cars
	int deltaTime = time - b->time;
	GameStateChange change = MoveCars(b, deltaTime);
	if (change.message != ChangeNoChange)
	{
		return change;
	}

	// stork fly
	change = StorkFly(b, deltaTime);
	if (change.message != ChangeNoChange)
	{
		return change;
	}

	b->time = time;

	if (time > b->maxTime * 1000)
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

void DrawBuildings(int upperStatusAreaSize, const Building* buildings, int buildingsSize)
{
	for (int i = 0; i < buildingsSize; ++i)
	{
		Building b = buildings[i];
		move(b.roadNumber + upperStatusAreaSize, b.x);
		StartPair(Brick_Black);
		printw("A");
		EndPair(Brick_Black);
	}
}

void DrawCar(const Board* board, int i, int UpperStatusAreaSize)
{
	constexpr char carsChars[4][4] = { "", "H", "HH", "HHH" };
	Car& c = board->cars[i];

	move(c.roadNumber + UpperStatusAreaSize, (int)round(c.x));
	StartPair(Window_Black);
	printw("X");
	EndPair(Window_Black);

	if(board->roads[c.roadNumber].direction == Right)
	{
		move(c.roadNumber + UpperStatusAreaSize, (int)round(c.x) - c.size);
	}

	switch (c.type)
	{
		case Friendly:
		{
			StartPair(Green_Black);
			printw(carsChars[c.size]);
			EndPair(Green_Black);
			break;
		}
		case Bad:
		{
			StartPair(Red_Black);
			printw(carsChars[c.size]);
			EndPair(Red_Black);
			break;
		}
		case Taxi:
		{
			StartPair(Yellow_Black);
			printw(carsChars[c.size]);
			EndPair(Yellow_Black);
			break;
		}
	}
}

void DrawFrog(const Board* board, int UpperStatusAreaSize)
{
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
}

void DrawStork(const Board* board, int UpperStatusAreaSize)
{
	StartPair(Red_White);
	move(board->stork.y + UpperStatusAreaSize, board->stork.x);
	printw("S");
	EndPair(Red_White);
}

void DrawHome(const Board* board, int UpperStatusAreaSize)
{
	move(board->home.y + UpperStatusAreaSize, board->home.x);
	StartPair(Black_Brick);
	printw("H");
	EndPair(Black_Brick);
}

void GameDraw(const GameState& self, WINDOW*win)
{
	constexpr int UpperStatusAreaSize = 1;
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
	for (int i = 0; i < board->carsSize; ++i)
	{
		DrawCar(board, i, UpperStatusAreaSize);
	}

	// frog
	DrawFrog(board, UpperStatusAreaSize);

	// buildings/obstacles
	DrawBuildings(UpperStatusAreaSize, board->buildings, board->buildingsSize);

	// stork
	DrawStork(board, UpperStatusAreaSize);

	// home
	DrawHome(board, UpperStatusAreaSize);

	wrefresh(win);
}

void GameDone(GameState& self, void* initData)
{
	//delete self.data;
	delete initData;
}

void InitRoads(Board* board, int roadSize)
{
	board->roadsSize = roadSize;
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
}

void InitFrog(Board* board)
{
	int x = rand() % board->width / 2;
	board->frog = { x, board->roadsSize - 1, 0, NULL, false };
}

void InitCars(Board* board, int carsSize)
{
	board->carsSize = carsSize;
	board->cars = new Car[board->carsSize];
	board->cars[0] = { 3, 4.0f, -1, 1, Normal, Friendly };
	board->cars[1] = { 2, 2.5f, -1, 2, Normal, Bad };
	board->cars[2] = { 1, 2.7f, 0, 5, Normal, Bad };
	board->cars[3] = { 2, 5.5f, -1, 6, Normal, Taxi };
	board->cars[4] = { 3, 8.0f, -1, 7, Normal, Friendly };
}

void InitBuildings(Board* board, int buildingsSize)
{
	board->buildingsSize = buildingsSize;
	board->buildings = new Building[board->buildingsSize];
	board->buildings[0] = { 5, 3 };
	board->buildings[1] = { 1, 3 };
	board->buildings[2] = { 5, 4 };
	board->buildings[3] = { 4, 4 };
	board->buildings[4] = { 5, 8 };
}

void InitOtherVariables(Board* board)
{
	int homeY = rand() % board->width;
	board->home = { homeY, 0 };
	board->score = 0;
	board->time = 0;
	board->maxTime = 100;
}

void InitStork(Board* board)
{
	board->stork = { 0, 0, 0.0f, 1.0f };
}

void GameInit(GameState& self, void* initData)
{
	Board* board = new Board;
	board->width = 80;

	InitRoads(board, 10);

	InitFrog(board);

	InitCars(board, 5);

	InitBuildings(board, 5);

	InitOtherVariables(board);

	InitStork(board);

	int cols = board->width + 1;
	int lines = board->roadsSize + 2;

	Resize(cols, lines);

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
	game.data = NULL;
	return game;
}

// Game over

GameStateChange GameOverKeysHandler(const GameState& self, int key)
{
	GameOverMessageData* data = (GameOverMessageData*)self.data;

	if ((!data->won || data->players[4].score >= data->score) && key == 'q')
	{
		return { ChangeToStart, NULL };
	}

	switch (key)
	{
		case 'q':
		{
			if (data->enter && strcmp(data->str, "") != 0)
			{
				return { ChangeToStart, NULL };
			}
			break;
		}
		case '>':
		{
			if (data->str[0] != '\0')
			{
				data->enter = true;
			}
			return { ChangeNoChange, NULL };
		}
		case '<':
		{
				if (!data->enter)
				{
					data->str[--data->index] = '\0';

					if (data->index < 0)
					{
						data->index = 0;
					}
				}
			

			return { ChangeNoChange, NULL };
		}
	default: break;
	}

	if (key >= 'a' && key <= 'z')
	{
		if (data->index < 10)
		{
			data->str[data->index] = char(key);
			data->index++;
			return { ChangeNoChange, NULL };
		}
	}
	return { ChangeNoChange, NULL };
}

GameStateChange GameOverTimerHandler(const GameState& self, int time)
{
	return { ChangeNoChange, NULL };
}

void DrawStr(const char* chr, int length)
{
	for (int i = 0; i < length; ++i)
	{
		printw(chr);
	}
}

void DrawTable(GameOverMessageData* data)
{
	printw("                      Best scores:                     \n");
	printw("                  +------------+-----+                  ");

	for (int i = 0; i < 5; ++i)
	{
		printw("                  | % 10s | %03d |                  ", data->players[i].name, data->players[i].score);
		printw("                  +------------+-----+                  ");
	}
	printw("\n");
}

void DrawWon()
{
	printw("\n");
	printw("        Y   Y   OO   U  U    W   W   OO   N   N\n");
	printw("         Y Y   O  O  U  U    W   W  O  O  NN  N\n");
	printw("          Y    O  O  U  U    W   W  O  O  N N N\n");
	printw("          Y    O  O  U  U    W W W  O  O  N  NN\n");
	printw("          Y     OO    UU      W W    OO   N   N\n");
}

void EnterName(GameOverMessageData* data)
{
	if (data->players[4].score < data->score)
	{
		printw("\n");
		printw("                      Enter name:\n");
		printw("                    '>' => enter\n");
		printw("                    '<' => backspace\n");
		printw("                       %s", data->str);

		if (!data->enter)
		{
			printw("_");
		}
		printw("\n");
	}
}

void DrawYouWon1(GameOverMessageData* data)
{
	DrawWon();
	printw("\n\n"); StartPair(Brick_Black);
	DrawStr(" ", 11); printw("_________________________|##|_____\n");
	printw("          /"); DrawStr("#", 25); printw("|##|#####\\\n");
	printw("         /##########################+--+######\\\n");
	printw("        /"); DrawStr("#", 38); printw("\\\n");
	printw("       /"); DrawStr("#", 40); printw("\\\n");
	printw("      /+"); DrawStr("-", 40); printw("+\\\n");
	printw("     / |"); DrawStr(" ", 40); printw("| \\ \n");
	printw("       |"); DrawStr(" ", 40); printw("|\n");
	printw("       |  +------+                              |\n"); EndPair(Brick_Black);
}

void DrawYouWon2(GameOverMessageData* data)
{
	StartPair(Brick_Black); printw("       |  |      |             "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw("(.)_(.)"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("          |\n"); EndPair(Brick_Black);
	StartPair(Brick_Black); printw("       |  |      |          "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw("_ (  ,_,  ) _"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("       |\n"); EndPair(Brick_Black);
	StartPair(Brick_Black); printw("       |  |    o |         "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw(
		R"(/ \/`-----'\/ \)"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("      |\n"); EndPair(Brick_Black);
	StartPair(Brick_Black); printw("       |  |      |       "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw("__\\ ( (     ) ) /__"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("    |\n"); EndPair(Brick_Black);
}

void DrawYouWon3(GameOverMessageData* data)
{
	StartPair(Brick_Black); printw("       |  |      |       "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw(
		R"()   /\ \._./ /\   ()"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("    |\n"); EndPair(Brick_Black);
	StartPair(Brick_Black); printw("       |  |      |        "); EndPair(Brick_Black); StartPair(FrogGreen_Black); printw(
		R"()_/ /|\   /|\ \_()"); EndPair(FrogGreen_Black); StartPair(Brick_Black); printw("     |\n"); EndPair(Brick_Black);
	StartPair(GrassGreen_Black); DrawStr("~", 55); EndPair(GrassGreen_Black);
	printw("                    Your score: %d\n", data->score);

	EnterName(data);
}

void DrawLost()
{
	printw("\n");
	printw("      Y   Y   OO   U  U    L     OO    SS   TTTTT\n");
	printw("       Y Y   O  O  U  U    L    O  O  S       T\n");
	printw("        Y    O  O  U  U    L    O  O   SS     T\n");
	printw("        Y    O  O  U  U    L    O  O     S    T\n");
	printw("        Y     OO    UU     LLL   OO    SS     T\n");
}

void DrawYouLost1(GameOverMessageData* data)
{
	DrawLost();
	printw("\n");
	printw("                                  +--------------------\n");
	printw("                                 //---------++---------\n");
	printw("                                /"); StartPair(Window_Black); printw("/##########"); EndPair(Window_Black); printw("||"); StartPair(Window_Black); printw("#########"); EndPair(Window_Black); printw("\n");
	printw("                               /"); StartPair(Window_Black); printw("/###########"); EndPair(Window_Black); printw("||"); StartPair(Window_Black); printw("#########"); EndPair(Window_Black); printw("\n");
	printw("                              /"); StartPair(Window_Black); printw("/############"); EndPair(Window_Black); printw("||"); StartPair(Window_Black); printw("#########"); EndPair(Window_Black); printw("\n");
	printw("                             //-------------++---------\n");
	StartPair(Yellow_Black); printw("\\"); EndPair(Yellow_Black); printw("+--------------------------++--------------++---------\n");
}

void DrawYouLost2(GameOverMessageData* data)
{
	StartPair(Yellow_Black); printw("-###"); EndPair(Yellow_Black); printw("                         |           O  ||         \n");
	StartPair(Yellow_Black); printw("-###"); EndPair(Yellow_Black); printw("       ______            |              ||         \n");
	StartPair(Yellow_Black); printw("/"); EndPair(Yellow_Black); printw("|       / ,-~-, \\           |              ||         \n");
	StartPair(FrogBlood_Black); printw(" |"); EndPair(FrogBlood_Black);  StartPair(FrogBlood_FrogBlood); printw("  "); EndPair(FrogBlood_FrogBlood);  printw("    // \\   / \\\\          |              ||\n");
	StartPair(FrogBlood_Black); printw(" |"); EndPair(FrogBlood_Black); StartPair(FrogBlood_FrogBlood); printw("   "); EndPair(FrogBlood_FrogBlood); printw("  |,   \\ /   ,|         |              ||\n");
}

void DrawYouLost3(GameOverMessageData* data)
{
	StartPair(FrogBlood_Black); printw(" +-----+|--"); EndPair(FrogBlood_Black); printw("--O----|+-----------------------------------\n");
	StartPair(FrogBlood_Black); printw("        \\"); EndPair(FrogBlood_Black); printw("   / \\   /                                    \n");
	StartPair(FrogBlood_Black); printw("         ',"); EndPair(FrogBlood_Black); printw("/   \\,'                                     \n");
	StartPair(FrogBlood_Black); printw("           '-~-'"); EndPair(FrogBlood_Black); printw("                                       \n");
	StartPair(FrogBlood_FrogBlood); printw("-------------------------"); EndPair(FrogBlood_FrogBlood); StartPair(RoadGray_RoadGray); printw("------------------------------\n\n"); EndPair(RoadGray_RoadGray);
}

void GameOverDraw(const GameState& self, WINDOW*win)
{
	GameOverMessageData* data = (GameOverMessageData*)self.data;

	clear();
	if (data->won)
	{
		DrawYouWon1(data);
		DrawYouWon2(data);
		DrawYouWon3(data);

		DrawTable(data);

		if (data->enter || data->players[4].score >= data->score)
		{
			printw("                 q => quit to main menu\n");
		}
	}
	else
	{
		DrawYouLost1(data);
		DrawYouLost2(data);
		DrawYouLost3(data);

		DrawTable(data);

		printw("                 q => quit to main menu\n");
	}
	wrefresh(win);
}

void GameOverDone(GameState& self, void* initData)
{
	GameOverMessageData* data = (GameOverMessageData*)initData;
	FILE* file = fopen("best.txt", "w");

	for (int i = 0; i < 5; ++i)
	{
		if (strcmp(data->players[i].name, "---") == 0)
		{
			fprintf(file, "%s\n", "(null)");
		}
		else
		{
			if (strcmp(data->players[i].name, "You") == 0)
			{
				fprintf(file, "%s\n", data->str);
				fprintf(file, "%d\n", data->score);
			}
			else
			{
				fprintf(file, "%s\n", data->players[i].name);
				fprintf(file, "%d\n", data->players[i].score);
			}
		}
	}

	fclose(file);

	delete data;
}

void InsertYou(GameOverMessageData* data)
{
	for (int i = 5 - 1; i >= 0; --i)
	{
		if (data->players[i].score >= data->score)
		{
			data->players[i + 1].score = 0;
			strcpy(data->players[i + 1].name, "You");
			return;
		}
	}
	data->players[0].score = 0;
	strcpy(data->players[0].name, "You");
}

void GameOverInit(GameState& self, void* initData)
{
	GameOverMessageData* data = (GameOverMessageData*)initData;
	data->enter = false;
	strcpy(data->str, "\0");
	data->index = 0;
	self.data = data;

	FILE* file = fopen("best.txt", "r");
	if (file == NULL)
	{
		// if file do not exist
		for (int i = 0; i < 5; ++i)
		{
			//strcpy(data->players[i].name, "1234567890\0");
			strcpy(data->players[i].name, "---\0");
			data->players[i].score = 0;
		}
	}
	else
	{
		//if file exists
		// read values

		for (int i = 0; i < 5; ++i)
		{
			int a = fscanf(file, "%s\n", data->players[i].name);
			if (strcmp(data->players[i].name, "(null)") == 0 || a != 1)
			{
				strcpy(data->players[i].name, "---\0");
			}
			a = fscanf(file, "%d\n", &data->players[i].score);
			if (a != 1)
			{
				data->players[i].score = 0;
			}
		}

		fclose(file);
	}

	InsertYou(data);

	Resize(56, 45);
}

GameState CreateGameOver()
{
	GameState gameOver;
	gameOver.init = &GameOverInit;
	gameOver.keysHandler = &GameOverKeysHandler;
	gameOver.timerHandler = &GameOverTimerHandler;
	gameOver.draw = &GameOverDraw;
	gameOver.done = &GameOverDone;
	gameOver.data = NULL;
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

GameStateChange MainLoop(const GameState& current, WINDOW* win)
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
	srand(time(NULL));  // NOLINT(cert-msc51-cpp, clang-diagnostic-shorten-64-to-32)
	WINDOW* win = InitWindow();

	GameState Start = CreateStart();
	GameState Game = CreateGame();
	GameState GameOver = CreateGameOver();

	GameState current = Start;
	current.init(current, NULL);
	
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
			case ChangeNoChange:
				break;
		}
		current.init(current, change.data);
	}
}

// dodac referencje
// przy delete zrzutowac wskaznik przed delete
