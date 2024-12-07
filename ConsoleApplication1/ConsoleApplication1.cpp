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
#include <io.h>
// enums

enum ColorPairs
{
	GrassPair = 12,
	FrogPair = 14,
	RoadPair = 16,
	HomePair = 18,
	TaxiPair = 20,
	FrogBloodFontPair = 22,
	BloodPair = 24,
	WindowPair = 26,
	BrickPair = 28,
	GrassHomePair = 30,
	BadCarPair = 32,
	FriendlyCarPair = 34,
	StorkPair = 36,
	CarLightsPair = 38,
	BuildingPair = 40
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
	ExitProgram = 4,
	ChangeToLevel = 5,
	ChangeToReplay = 6,
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

// options

struct HomeOptions
{
	char* skin;
};

struct CarOptions
{
	int speedUpFactor;
	int speedUpChances;
	int slowDownChances;
	int returnChances;

	int wrapChances;
	int breakDistance;

	int carsNumber;

	int maxSpeed;
	int minSpeed;

	char* bikeSkin;
	char* carSkin;
	char* truckSkin;
};

struct FrogOptions
{
	char* skinOne;
	char* skinTwo;
	int jumpTime;
};

struct StorkOptions
{
	int startX, startY;
	float speed;
	char* skin;
};

struct BoardOptions
{
	int width;
};

struct RoadOptions
{
	int roadNumber;
	int streets;
	int grass;
};

struct BuildingOptions
{
	int buildingsNumber;
	char* skin;
};

struct RGB
{
	short r, g, b;
};

struct ColorsOptions
{
	RGB GrassFont, GrassBack;
	RGB FrogFont, FrogBack;
	RGB RoadFont, RoadBack;
	RGB HomeFont, HomeBack;
	RGB TaxiFont, TaxiBack;
	RGB FrogBloodFontFont, FrogBloodFontBack;
	RGB BloodFont, BloodBack;
	RGB WindowFont, WindowBack;
	RGB BrickFont, BrickBack;
	RGB GrassHomeFont, GrassHomeBack;
	RGB BadCarFont, BadCarBack;
	RGB FriendlyCarFont, FriendlyCarBack;
	RGB StorkFont, StorkBack;
	RGB CarLightFont, CarLightBack;
	RGB BuildingFont, BuildingBack;
};

struct GeneralOptions
{
	int startScreenWidth, startScreenHeight;
	int gameOverScreenWidth, gameOverScreenHeight;
	int maxTime;
};

struct FilesOptions
{
	char* bestScoresFileName;
	char* self;
};

struct Options
{
	HomeOptions home;
	CarOptions car;
	FrogOptions frog;
	StorkOptions stork;
	BoardOptions board;
	RoadOptions road;
	BuildingOptions building;
	ColorsOptions colors;
	GeneralOptions general;
	FilesOptions files;

	bool useSeed;
	int seed;
};

// structs

struct GameStateChange
{
	GameStateMessage message;
	void* data;
};

struct GameState
{
	void (*init)(GameState&, const Options*, void*);
	GameStateChange(*keysHandler)(const GameState&, int);
	GameStateChange(*timerHandler)(const GameState&, const Options*, int);
	void (*draw)(const GameState&, const Options*, WINDOW*);
	void (*done)(GameState&, const Options*, void*);

	Options* options;
	void* data;
	bool recordable;
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

	int levelId;

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
	int jumpTime;

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
	int roadsNumber;
	int width;
	int score;
	int time; // in miliseconds
	int lastFrogJump;
	int maxTime;// in seconds
	Car* cars;
	int carsNumber;
	Building* buildings;
	int buildingsNumber;
	Stork stork;
	int levelId;
};

struct ChangeLevelData
{
	int levelId;
	int score;
};

struct Event
{
	int time;
	char eventType; // k => key; t => timer
	int key; // is used only if eventType=='k'
};

struct Recorder
{
	char* optionsName;
	int seed;
	Event* events;
	int eventsCount;
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

void InitColors1(const ColorsOptions& colors)
{
	InitColor(GrassPair, colors.GrassFont.r, colors.GrassFont.g, colors.GrassFont.b);
	InitColor(GrassPair + 1, colors.GrassBack.r, colors.GrassBack.g, colors.GrassBack.b);
	InitColorPair(GrassPair, GrassPair, GrassPair + 1);

	InitColor(FrogPair, colors.FrogFont.r, colors.FrogFont.g, colors.FrogFont.b);
	InitColor(FrogPair + 1, colors.FrogBack.r, colors.FrogBack.g, colors.FrogBack.b);
	InitColorPair(FrogPair, FrogPair, FrogPair + 1);

	InitColor(RoadPair, colors.RoadFont.r, colors.RoadFont.g, colors.RoadFont.b);
	InitColor(RoadPair + 1, colors.RoadBack.r, colors.RoadBack.g, colors.RoadBack.b);
	InitColorPair(RoadPair, RoadPair, RoadPair + 1);

	InitColor(HomePair, colors.HomeFont.r, colors.HomeFont.g, colors.HomeFont.b);
	InitColor(HomePair + 1, colors.HomeBack.r, colors.HomeBack.g, colors.HomeBack.b);
	InitColorPair(HomePair, HomePair, HomePair + 1);

	InitColor(TaxiPair, colors.TaxiFont.r, colors.TaxiFont.g, colors.TaxiFont.b);
	InitColor(TaxiPair + 1, colors.TaxiBack.r, colors.TaxiBack.g, colors.TaxiBack.b);
	InitColorPair(TaxiPair, TaxiPair, TaxiPair + 1);
}

void InitColors2(const ColorsOptions& colors)
{
	InitColor(FrogBloodFontPair, colors.FrogBloodFontFont.r, colors.FrogBloodFontFont.g, colors.FrogBloodFontFont.b);
	InitColor(FrogBloodFontPair + 1, colors.FrogBloodFontBack.r, colors.FrogBloodFontBack.g, colors.FrogBloodFontBack.b);
	InitColorPair(FrogBloodFontPair, FrogBloodFontPair, FrogBloodFontPair + 1);

	InitColor(BloodPair, colors.BloodFont.r, colors.BloodFont.g, colors.BloodFont.b);
	InitColor(BloodPair + 1, colors.BloodBack.r, colors.BloodBack.g, colors.BloodBack.b);
	InitColorPair(BloodPair, BloodPair, BloodPair + 1);

	InitColor(WindowPair, colors.WindowFont.r, colors.WindowFont.g, colors.WindowFont.b);
	InitColor(WindowPair + 1, colors.WindowBack.r, colors.WindowBack.g, colors.WindowBack.b);
	InitColorPair(WindowPair, WindowPair, WindowPair + 1);

	InitColor(BrickPair, colors.BrickFont.r, colors.BrickFont.g, colors.BrickFont.b);
	InitColor(BrickPair + 1, colors.BrickBack.r, colors.BrickBack.g, colors.BrickBack.b);
	InitColorPair(BrickPair, BrickPair, BrickPair + 1);
}

void InitColors3(const ColorsOptions& colors)
{
	InitColor(GrassHomePair, colors.GrassHomeFont.r, colors.GrassHomeFont.g, colors.GrassHomeFont.b);
	InitColor(GrassHomePair + 1, colors.GrassHomeBack.r, colors.GrassHomeBack.g, colors.GrassHomeBack.b);
	InitColorPair(GrassHomePair, GrassHomePair, GrassHomePair + 1);

	InitColor(BadCarPair, colors.BadCarFont.r, colors.BadCarFont.g, colors.BadCarFont.b);
	InitColor(BadCarPair + 1, colors.BadCarBack.r, colors.BadCarBack.g, colors.BadCarBack.b);
	InitColorPair(BadCarPair, BadCarPair, BadCarPair + 1);

	InitColor(FriendlyCarPair, colors.FriendlyCarFont.r, colors.FriendlyCarFont.g, colors.FriendlyCarFont.b);
	InitColor(FriendlyCarPair + 1, colors.FriendlyCarBack.r, colors.FriendlyCarBack.g, colors.FriendlyCarBack.b);
	InitColorPair(FriendlyCarPair, FriendlyCarPair, FriendlyCarPair + 1);

	InitColor(StorkPair, colors.StorkFont.r, colors.StorkFont.g, colors.StorkFont.b);
	InitColor(StorkPair + 1, colors.StorkBack.r, colors.StorkBack.g, colors.StorkBack.b);
	InitColorPair(StorkPair, StorkPair, StorkPair + 1);
}

void InitColors4(const ColorsOptions& colors)
{
	InitColor(CarLightsPair, colors.CarLightFont.r, colors.CarLightFont.g, colors.CarLightFont.b);
	InitColor(CarLightsPair + 1, colors.CarLightBack.r, colors.CarLightBack.g, colors.CarLightBack.b);
	InitColorPair(CarLightsPair, CarLightsPair, CarLightsPair + 1);

	InitColor(BuildingPair, colors.BuildingFont.r, colors.BuildingFont.g, colors.BuildingFont.b);
	InitColor(BuildingPair + 1, colors.BuildingBack.r, colors.BuildingBack.g, colors.BuildingBack.b);
	InitColorPair(BuildingPair, BuildingPair, BuildingPair + 1);
}

WINDOW* InitWindow(const ColorsOptions& colors)
{
	// curses init
	WINDOW*win = initscr();
	curs_set(0);

	// colors init
	if (has_colors())
	{
		start_color();
		InitColors1(colors);
		InitColors2(colors);
		InitColors3(colors);
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
			ChangeLevelData* data = new ChangeLevelData{ 0 };
			return { ChangeToGame, data };
		}
		case 'l':
		{
			ChangeLevelData* data = new ChangeLevelData{ 1 };
			return { ChangeToLevel, data };
		}
		case 'r':
		{
			ChangeLevelData* data = new ChangeLevelData{ 0 };
			return {ChangeToReplay, data};
		}
		default:
		{
			return { ChangeNoChange, NULL };
		}
	}
}

GameStateChange StartTimerHandler(const GameState& self, const Options* options, int time)
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

void StartDraw(const GameState& self, const Options* options, WINDOW* win)
{
	clear();
	StartPair(FrogPair);
	printw("     JJJ  U  U  M     M  PPP   I  N   N   GG\n");
	printw("       J  U  U  MM   MM  P  P  I  NN  N  G  G\n");
	printw("       J  U  U  M M M M  PPP   I  N N N  G\n");
	printw("       J  U  U  M  M  M  P     I  N  NN  G  GG\n");
	printw("     JJ    UU   M     M  P     I  N   N   GG\n");
	printw("\n");
	printw("              FFF  RRR    OO    GG\n");
	printw("              F    R  R  O  O  G  G\n");
	printw("              FFF  RRR   O  O  G\n");
	printw("              F    R R   O  O  G  GG\n");
	printw("              F    R  R   OO    GG\n");
	printw("\n");
	DrawStartFrog();
	printw("\n");
	printw("               s => start new game\n");
	printw("               l => start level 1\n");
	printw("               r => run last game\n");
	printw("               q => quit program\n");
	EndPair(FrogPair);
	wrefresh(win);
}

void StartDone(GameState& self, const Options* options, void* initData)
{
	// uncomment if start has any initdata
	//delete self.data;
	//delete initData;
}

void StartInit(GameState& init, const Options* options, void* initData)
{
	Resize(init.options->general.startScreenWidth, init.options->general.startScreenHeight);
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
	if (newY > b->roadsNumber - 1)
	{
		return false;
	}
	if (newX > b->width - 1)
	{
		return false;
	}

	for (int i = 0; i < b->buildingsNumber; ++i)
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

	if (board->time < board->lastFrogJump + board->frog.jumpTime)
	{
		return { ChangeNoChange, NULL };
	}

	switch (key)
	{
		case 'q':
		{
			return { ChangeToStart, NULL };
		}
		case 'w':
		{
			jump = CanFrogJump(board->frog.x, board->frog.y - 1, board);
			board->lastFrogJump = board->time;
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
			board->lastFrogJump = board->time;
			if (jump)
			{
				board->frog.x--;
			}
			break;
		}
		case 's':
		{
			jump = CanFrogJump(board->frog.x, board->frog.y + 1, board);
			board->lastFrogJump = board->time;
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
			board->lastFrogJump = board->time;
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
		GameOverMessageData* data = new GameOverMessageData{ true, board->score, board->levelId };
		return { ChangeToGameOver, data };
	}
	return { ChangeNoChange, NULL };
}

bool IsFrogHitted(const Frog& f, const Car& c)
{
	// TODO: czy dla szybkich samochodow, nie zdazy sie tak, ze przeskoczy on nad zaba
	if (c.type == Bad && !f.onCar && f.x == (int)round(c.x) && f.y == c.roadNumber)
	{
		return true;
	}
	return false;
}

void ChangeCarSpeed(Car& c, const CarOptions& options)
{
	// speed up cars
	int speedChange = rand() % 100;
	float speedUpFactor = options.speedUpFactor;

	if (c.speedType == Normal)
	{ // change car speed
		if (speedChange < options.speedUpChances) // x% of chances
		{ // speed up
			c.speed *= speedUpFactor;
			c.speedType = Fast;
		}
		else if (speedChange < options.speedUpChances + options.slowDownChances) // x% of chances
		{ // slow down
			c.speed *= 1.0f / speedUpFactor;
			c.speedType = Slow;
		}
	}
	else if (speedChange < options.returnChances) // x% of chances
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

int GetNumberOfStreets(const Board* b, RoadType road)
{
	int numberOfStreets = 0;
	for (int i = 0; i < b->roadsNumber; ++i)
	{
		if (b->roads[i].type == road)
		{
			numberOfStreets++;
		}
	}
	return numberOfStreets;
}

int* GetIndexesOfStreets(const Board* b, int numberOfStreets, RoadType road)
{
	int* streets = new int[numberOfStreets];
	int index = 0;
	for (int i = 0; i < b->roadsNumber; ++i)
	{
		if (b->roads[i].type == road)
		{
			streets[index] = i;
			index++;
		}
	}
	return streets;
}

void ChangeCarRoad(const Board* b, Car& c, const Options* options)
{
	int numberOfStreets = GetNumberOfStreets(b, Street);
	int* streets = GetIndexesOfStreets(b, numberOfStreets, Street);

	c.x = 1.0f - c.size;
	int newRoad = rand() % numberOfStreets;
	c.roadNumber = streets[newRoad];
	c.size = rand() % 3 + 1;
	int max = options->car.maxSpeed;
	int min = options->car.minSpeed;
	c.speed = rand() % (max + 1 - min) + min;
	c.speedType = Normal;
	c.type = CarType(rand() % 3);

	delete[] streets;
}

void WrapLeft(Car& c, Board* b, const Options* options)
{
	int wrap = rand() % 100;
	if (wrap > options->car.wrapChances)
	{
		c.x = b->width - c.size + 1;
	}
	else
	{
		ChangeCarRoad(b, c, options);
	}
}

void WrapRight(Car& c, Board* b, const Options* options)
{
	int wrap = rand() % 100;
	if (wrap > options->car.wrapChances)
	{
		c.x = 1.0f - c.size;
	}
	else
	{
		ChangeCarRoad(b, c, options);
	}
}

GameStateChange MoveCars(Board* b, const Options* options, int deltaTime)
{
	for (int i = 0; i < b->carsNumber; ++i)
	{
		Car& c = b->cars[i];

		float f = deltaTime * c.speed / 1000.0f;

		switch (b->roads[b->cars[i].roadNumber].direction)
		{
			case Left:
			{
				int distance = (int)round(c.x) - b->frog.x;
				if (c.type == Friendly && 0 <= distance && distance <= options->car.breakDistance && c.roadNumber == b->frog.y)
				{
					break;
				}
				if (c.type == Taxi && (int)round(c.x) - 1 == b->frog.x && c.roadNumber == b->frog.y)
				{
					FrogGetInTaxi(b, c);
				}
				c.x -= f;
				if (c.x <= 0)
				{
					WrapLeft(c, b, options);
				}
				break;
			}
			case Right:
			{
				int distance = b->frog.x - (int)round(c.x);
				if (c.type == Friendly && 0 <= distance && distance <= options->car.breakDistance && c.roadNumber == b->frog.y)
				{
					break;
				}
				if (c.type == Taxi && (int)round(c.x) + 1 == b->frog.x && c.roadNumber == b->frog.y)
				{
					FrogGetInTaxi(b, c);
				}
				c.x += f;
				if (c.x >= b->width)
				{
					WrapRight(c, b, options);
				}
				break;
			}
		}
		ChangeCarSpeed(c, options->car);
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

GameStateChange GameTimerHandler(const GameState& self, const Options* options, int time)
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
	GameStateChange change = MoveCars(b, options, deltaTime);
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
	StartPair(RoadPair);
	for (int i = 0; i < width; ++i)
	{
		printw("=");
	}
	EndPair(RoadPair);
}

void DrawGrass(int width)
{
	StartPair(GrassPair);
	for (int i = 0; i < width; ++i)
	{
		printw(" ");
	}
	EndPair(GrassPair);
}

void DrawBuildings(int upperStatusAreaSize, const Building* buildings, const Options* options, int buildingsSize)
{
	for (int i = 0; i < buildingsSize; ++i)
	{
		Building b = buildings[i];
		move(b.roadNumber + upperStatusAreaSize, b.x);
		StartPair(BuildingPair);
		printw(options->building.skin);
		EndPair(BuildingPair);
	}
}

void DrawCar(const Board* board, const Options* options, int i, int UpperStatusAreaSize)
{
	char carsChars[4][4];
	strcpy(carsChars[0], "");
	strcpy(carsChars[1], options->car.bikeSkin);
	strcpy(carsChars[2], options->car.carSkin);
	strcpy(carsChars[3], options->car.truckSkin);
	Car& c = board->cars[i];

	move(c.roadNumber + UpperStatusAreaSize, (int)round(c.x));
	StartPair(WindowPair);
	printw("X");
	EndPair(WindowPair);

	if(board->roads[c.roadNumber].direction == Right)
	{
		move(c.roadNumber + UpperStatusAreaSize, (int)round(c.x) - c.size);
	}

	switch (c.type)
	{
		case Friendly:
		{
			StartPair(FriendlyCarPair);
			printw(carsChars[c.size]);
			EndPair(FriendlyCarPair);
			break;
		}
		case Bad:
		{
			StartPair(BadCarPair);
			printw(carsChars[c.size]);
			EndPair(BadCarPair);
			break;
		}
		case Taxi:
		{
			StartPair(TaxiPair);
			printw(carsChars[c.size]);
			EndPair(TaxiPair);
			break;
		}
	}
}

void DrawFrog(const Board* board, Options* options, int UpperStatusAreaSize)
{
	move(board->frog.y + UpperStatusAreaSize, board->frog.x);
	StartPair(FrogPair);
	if (board->frog.skin == 0)
	{
		printw(options->frog.skinOne);
	}
	else
	{
		printw(options->frog.skinTwo);
	}
	EndPair(FrogPair);
}

void DrawStork(const Board* board, const Options* options, int UpperStatusAreaSize)
{
	StartPair(StorkPair);
	move(board->stork.y + UpperStatusAreaSize, board->stork.x);
	printw(options->stork.skin);
	EndPair(StorkPair);
}

void DrawHome(const Board* board, const Options* options, int UpperStatusAreaSize)
{
	move(board->home.y + UpperStatusAreaSize, board->home.x);
	StartPair(HomePair);
	printw(options->home.skin);
	EndPair(HomePair);
}

void GameDraw(const GameState& self, const Options* options, WINDOW*win)
{
	clear();

	Board* board = (Board*)self.data;

	// upper status area containing inforamtion about time and score
	const int upperStatusAreaSize = 1;
	printw("Time left: %ds  Score: %d  Level: %d\n", board->maxTime - board->time/1000, board->score, board->levelId);
	
	// roads
	for (int i = 0; i < board->roadsNumber; ++i)
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
	for (int i = 0; i < board->carsNumber; ++i)
	{
		DrawCar(board, options, i, upperStatusAreaSize);
	}

	// frog
	DrawFrog(board, self.options, upperStatusAreaSize);

	// buildings/obstacles
	DrawBuildings(upperStatusAreaSize, board->buildings, options, board->buildingsNumber);

	// stork
	DrawStork(board, options, upperStatusAreaSize);

	// home
	DrawHome(board, options, upperStatusAreaSize);

	wrefresh(win);
}

void GameDone(GameState& self, const Options* options, void* initData)
{
	//delete self.data;
	delete initData;
}

void InitRoads(Board* board, const Options* options)
{
	board->roadsNumber = options->road.roadNumber;
	board->roads = new Road[board->roadsNumber];

	board->roads[0].type = Grass;
	for (int i = 1; i < board->roadsNumber - 1; ++i)
	{
		board->roads[i].direction = Direction(rand() % 2);
		board->roads[i].type = RoadType(rand() % 2);
	}
	board->roads[board->roadsNumber - 1].type = Grass;
}

void InitFrog(Board* board, const Options* options)
{
	int x = rand() % board->width / 2;
	board->frog = { x, board->roadsNumber - 1, 0, options->frog.jumpTime, NULL, false };
}

void InitCars(Board* board, const Options* options)
{
	board->carsNumber = options->car.carsNumber;
	board->cars = new Car[board->carsNumber];

	int numberOfStreets = GetNumberOfStreets(board, Street);
	int* streets = GetIndexesOfStreets(board, numberOfStreets, Street);

	for (int i = 0; i < board->carsNumber; ++i)
	{
		board->cars[i].size = rand() % 3 + 1;
		int max = options->car.maxSpeed;
		int min = options->car.minSpeed;
		board->cars[i].speed = rand() % (max + 1 - min) + min;
		board->cars[i].x = 0;
		board->cars[i].roadNumber = streets[rand() % numberOfStreets];
		board->cars[i].speedType = Normal;
		board->cars[i].type = CarType(rand() % 3);
	}

	delete[] streets;
}

void InitBuildings(Board* board, const Options* options)
{
	board->buildingsNumber = options->building.buildingsNumber;
	board->buildings = new Building[board->buildingsNumber];

	int numberOfGrass = GetNumberOfStreets(board, Grass);
	int* grass = GetIndexesOfStreets(board, numberOfGrass, Grass);

	for (int i = 0; i < board->buildingsNumber; ++i)
	{
		board->buildings[i].x = rand() % board->width;
		board->buildings[i].roadNumber = grass[rand() % numberOfGrass];
	}

	delete[] grass;
}

void InitOtherVariables(Board* board, const Options* options)
{
	int homeY = rand() % board->width;
	board->home = { homeY, 0 };
	board->time = 0;
	board->maxTime = options->general.maxTime;
	board->lastFrogJump = 0;
}

void InitStork(Board* board, const Options* options)
{
	board->stork = { options->stork.startX, options->stork.startY, 0.0f, options->stork.speed };
}

void GameInit(GameState& self, const Options* options, void* initData)
{
	Board* board = new Board;

	ChangeLevelData* level = (ChangeLevelData*)initData;
	board->levelId = level->levelId;
	board->score = level->score;
	
	board->width = options->board.width;

	InitRoads(board, options);

	InitFrog(board, options);

	InitCars(board, options);

	InitBuildings(board, options);

	InitOtherVariables(board, options);

	InitStork(board, options);

	int cols = board->width + 1;
	int lines = board->roadsNumber + 2;

	Resize(cols, lines);

	self.data = board;
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

GameStateChange GameOverTimerHandler(const GameState& self, const Options* options, int time)
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
	printw("\n\n"); StartPair(BrickPair);
	DrawStr(" ", 11); printw("_________________________|##|_____\n");
	printw("          /"); DrawStr("#", 25); printw("|##|#####\\\n");
	printw("         /##########################+--+######\\\n");
	printw("        /"); DrawStr("#", 38); printw("\\\n");
	printw("       /"); DrawStr("#", 40); printw("\\\n");
	printw("      /+"); DrawStr("-", 40); printw("+\\\n");
	printw("     / |"); DrawStr(" ", 40); printw("| \\ \n");
	printw("       |"); DrawStr(" ", 40); printw("|\n");
	printw("       |  +------+                              |\n"); EndPair(BrickPair);
}

void DrawYouWon2(GameOverMessageData* data)
{
	StartPair(BrickPair); printw("       |  |      |             "); EndPair(BrickPair); StartPair(FrogPair); printw("(.)_(.)"); EndPair(FrogPair); StartPair(BrickPair); printw("          |\n"); EndPair(BrickPair);
	StartPair(BrickPair); printw("       |  |      |          "); EndPair(BrickPair); StartPair(FrogPair); printw("_ (  ,_,  ) _"); EndPair(FrogPair); StartPair(BrickPair); printw("       |\n"); EndPair(BrickPair);
	StartPair(BrickPair); printw("       |  |    o |         "); EndPair(BrickPair); StartPair(FrogPair); printw(
		R"(/ \/`-----'\/ \)"); EndPair(FrogPair); StartPair(BrickPair); printw("      |\n"); EndPair(BrickPair);
	StartPair(BrickPair); printw("       |  |      |       "); EndPair(BrickPair); StartPair(FrogPair); printw("__\\ ( (     ) ) /__"); EndPair(FrogPair); StartPair(BrickPair); printw("    |\n"); EndPair(BrickPair);
}

void DrawYouWon3(GameOverMessageData* data)
{
	StartPair(BrickPair); printw("       |  |      |       "); EndPair(BrickPair); StartPair(FrogPair); printw(
		R"()   /\ \._./ /\   ()"); EndPair(FrogPair); StartPair(BrickPair); printw("    |\n"); EndPair(BrickPair);
	StartPair(BrickPair); printw("       |  |      |        "); EndPair(BrickPair); StartPair(FrogPair); printw(
		R"()_/ /|\   /|\ \_()"); EndPair(FrogPair); StartPair(BrickPair); printw("     |\n"); EndPair(BrickPair);
	StartPair(GrassHomePair); DrawStr("~", 55); EndPair(GrassHomePair);
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
	printw("                                /"); StartPair(WindowPair); printw("/##########"); EndPair(WindowPair); printw("||"); StartPair(WindowPair); printw("#########"); EndPair(WindowPair); printw("\n");
	printw("                               /"); StartPair(WindowPair); printw("/###########"); EndPair(WindowPair); printw("||"); StartPair(WindowPair); printw("#########"); EndPair(WindowPair); printw("\n");
	printw("                              /"); StartPair(WindowPair); printw("/############"); EndPair(WindowPair); printw("||"); StartPair(WindowPair); printw("#########"); EndPair(WindowPair); printw("\n");
	printw("                             //-------------++---------\n");
	StartPair(CarLightsPair); printw("\\"); EndPair(CarLightsPair); printw("+--------------------------++--------------++---------\n");
}

void DrawYouLost2(GameOverMessageData* data)
{
	StartPair(CarLightsPair); printw("-###"); EndPair(CarLightsPair); printw("                         |           O  ||         \n");
	StartPair(CarLightsPair); printw("-###"); EndPair(CarLightsPair); printw("       ______            |              ||         \n");
	StartPair(CarLightsPair); printw("/"); EndPair(CarLightsPair); printw("|       / ,-~-, \\           |              ||         \n");
	StartPair(FrogBloodFontPair); printw(" |"); EndPair(FrogBloodFontPair);  StartPair(BloodPair); printw("  "); EndPair(BloodPair);  printw("    // \\   / \\\\          |              ||\n");
	StartPair(FrogBloodFontPair); printw(" |"); EndPair(FrogBloodFontPair); StartPair(BloodPair); printw("   "); EndPair(BloodPair); printw("  |,   \\ /   ,|         |              ||\n");
}

void DrawYouLost3(GameOverMessageData* data)
{
	StartPair(FrogBloodFontPair); printw(" +-----+|--"); EndPair(FrogBloodFontPair); printw("--O----|+-----------------------------------\n");
	StartPair(FrogBloodFontPair); printw("        \\"); EndPair(FrogBloodFontPair); printw("   / \\   /                                    \n");
	StartPair(FrogBloodFontPair); printw("         ',"); EndPair(FrogBloodFontPair); printw("/   \\,'                                     \n");
	StartPair(FrogBloodFontPair); printw("           '-~-'"); EndPair(FrogBloodFontPair); printw("                                       \n");
	StartPair(BloodPair); printw("-------------------------"); EndPair(BloodPair); StartPair(RoadPair); printw("------------------------------\n\n"); EndPair(RoadPair);
}

void GameOverDraw(const GameState& self, const Options* options, WINDOW*win)
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

void GameOverDone(GameState& self, const Options* options, void* initData)
{
	GameOverMessageData* data = (GameOverMessageData*)initData;
	FILE* file = fopen(options->files.bestScoresFileName, "w");

	if (file == NULL)
	{
		return;
	}

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

void GameOverInit(GameState& self, const Options* options, void* initData)
{
	GameOverMessageData* data = (GameOverMessageData*)initData;
	data->enter = false;
	strcpy(data->str, "\0");
	data->index = 0;
	self.data = data;

	FILE* file = fopen(options->files.bestScoresFileName, "r");
	if (file == NULL)
	{
		// if file do not exist
		for (int i = 0; i < 5; ++i)
		{
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
			int a = fscanf (file, "%s\n", data->players[i].name);
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

	Resize(options->general.gameOverScreenWidth, options->general.gameOverScreenHeight);
}

// read options

int ReadKeys()
{
	if (_kbhit())
	{
		int input = getchar();
		return input;
	}
	return NULL;
}

Recorder* LoadRec()
{
	FILE* file = fopen("rec.txt", "r");
	if (file == NULL)
	{
		return NULL;
	}

	Recorder* rec = new Recorder;

	fscanf(file, "%d\n", &rec->seed);

	rec->optionsName = new char[21];
	fgets(rec->optionsName, 20, file);
	int len = strlen(rec->optionsName);
	if (rec->optionsName[len - 1] == '\n')
	{
		rec->optionsName[len - 1] = '\0';
	}

	fscanf(file, "%d\n", &rec->eventsCount);
	rec->events = new Event[rec->eventsCount];

	for (int i = 0; i < rec->eventsCount; ++i)
	{
		fscanf(file, "%d: %c %d\n", &rec->events[i].time, &rec->events[i].eventType, &rec->events[i].key);
	}

	fclose(file);

	return rec;
}

void SaveRec(Recorder* rec)
{
	FILE* file = fopen("rec.txt", "w");
	if (file == NULL)
	{
		return;
	}

	fprintf(file, "%d\n", rec->seed);
	fprintf(file, "%s\n", rec->optionsName);
	fprintf(file, "%d\n", rec->eventsCount);
	for (int i = 0; i < rec->eventsCount; ++i)
	{
		fprintf(file, "%d: %c %d\n", rec->events[i].time, rec->events[i].eventType, rec->events[i].key);
	}

	fclose(file);
}

GameStateChange MainLoopPlayer(const GameState& current, const Options* options, Recorder* rec, WINDOW* win)
{
	int currentEventNo = 0;
	clock_t startTime = clock();

	while (true)
	{
		GameStateChange change;
		clock_t now = clock();
		int time = (now - startTime) * 1000 / CLOCKS_PER_SEC;

		Event currentEvent = rec->events[currentEventNo];
		if (time < currentEvent.time)
		{
			continue;
		}

		if (currentEvent.eventType == 'k')
		{
			change = current.keysHandler(current, currentEvent.key);
			if (change.message != ChangeNoChange)
			{
				return change;
			}
		}

		if (currentEvent.eventType == 't')
		{
			change = current.timerHandler(current, options, currentEvent.time);
			if (change.message != ChangeNoChange)
			{
				return change;
			}
		}

		current.draw(current, options, win);

		currentEventNo++;
	}
}

GameStateChange MainLoopRecordable(const GameState& current, const Options* options, WINDOW* win)
{
	clock_t startTime = clock();
	clock_t lastTimerEvent = 0;

	Recorder* rec = NULL;

	if (current.recordable)
	{
		rec = new Recorder;
		rec->seed = options->seed;
		rec->optionsName = options->files.self;
		rec->events = new Event[options->general.maxTime * 10 * 2];
		rec->eventsCount = 0;
	}

	while (true)
	{
		GameStateChange change;
		clock_t now = clock();
		int time = (now - startTime) * 1000 / CLOCKS_PER_SEC;

		int key = ReadKeys();
		if (key != NULL)
		{
			if (current.recordable)
			{
				rec->events[rec->eventsCount++] = { time, 'k', key };
			}

			change = current.keysHandler(current, key);
			if (change.message != ChangeNoChange)
			{
				if (change.message == ChangeToGameOver && current.recordable)
				{
					SaveRec(rec);
					delete rec->events;
					delete rec;
				}

				return change;
			}
		}

		int delta = time - lastTimerEvent;
		if (delta > 100)
		{
			if (current.recordable)
			{
				rec->events[rec->eventsCount++] = { time, 't', 0 };
			}

			lastTimerEvent = time;
			change = current.timerHandler(current, options, time);
			if (change.message != ChangeNoChange)
			{
				if (change.message == ChangeToGameOver && current.recordable)
				{
					SaveRec(rec);
					delete[] rec->events;
					delete rec;
				}

				return change;
			}
		}

		current.draw(current, options, win);
	}
}

GameStateChange MainLoop(const GameState& current, const Options* options, Recorder* rec, WINDOW* win)
{
	if (rec == NULL)
	{
		return MainLoopRecordable(current, options, win);
	}
	else
	{
		return MainLoopPlayer(current, options, rec, win);
	}
}

void CarVariablesInitialization(Options* options)
{
	options->car.speedUpFactor = 5;
	options->car.speedUpChances = 10;
	options->car.slowDownChances = 10;
	options->car.returnChances = 10;
	options->car.wrapChances = 50;
	options->car.breakDistance = 2;
	options->car.carsNumber = 10;
	options->car.minSpeed = 5;
	options->car.maxSpeed = 15;

	options->car.bikeSkin = new char[2];
	strcpy(options->car.bikeSkin, "#");

	options->car.carSkin = new char[3];
	strcpy(options->car.carSkin, "##");

	options->car.truckSkin = new char[4];
	strcpy(options->car.truckSkin, "###");
}

void VariablesInitialization(Options* options)
{
	options->general.startScreenWidth = 53;
	options->general.startScreenHeight = 23;
	options->general.gameOverScreenWidth = 56;
	options->general.gameOverScreenHeight = 45;
	options->general.maxTime = 100;

	options->frog.skinOne = new char[2];
	strcpy(options->frog.skinOne, "F");
	options->frog.skinTwo = new char[2];
	strcpy(options->frog.skinTwo, "f");
	options->frog.jumpTime = 100;

	options->home.skin = new char[2];
	strcpy(options->home.skin, "H");

	CarVariablesInitialization(options);

	options->road.roadNumber = 10;

	options->building.buildingsNumber = 10;
	options->building.skin = new char[2];
	strcpy(options->building.skin, "A");

	options->stork.startX = 0;
	options->stork.startY = 0;
	options->stork.speed = 1.0f;
	options->stork.skin = new char[2];
	strcpy(options->stork.skin, "S");

	options->board.width = 80;

	options->useSeed = false;
	options->seed = 1;

	options->files.bestScoresFileName = new char[9];
	strcpy(options->files.bestScoresFileName, "best.txt");

	options->files.self = new char[20];
	strcpy(options->files.self, "");
}

void FrogRelatedColorsInitializationAndGrass(Options* options)
{
	options->colors.FrogFont = { 153, 198, 142 };
	options->colors.FrogBack = { 0, 0, 0 };

	options->colors.FrogBloodFontFont = { 120, 6, 6 };
	options->colors.FrogBloodFontBack = { 0, 0, 0 };

	options->colors.BloodFont = { 120, 6, 6 };
	options->colors.BloodBack = { 120, 6, 6 };

	options->colors.GrassFont = { 65,152,10 };
	options->colors.GrassBack = { 65, 152, 10 };
}

Options* CreateOptions()
{
	Options* options = new Options;
	// variables initialization
	VariablesInitialization(options);
	// Colors initialization
	FrogRelatedColorsInitializationAndGrass(options);

	options->colors.RoadFont = { 179, 179, 179 };
	options->colors.RoadBack = { 179, 179, 179 };

	options->colors.HomeFont = { 255, 0, 0 };
	options->colors.HomeBack = { 65, 152, 10 };

	options->colors.TaxiFont = { 255, 255, 0 };
	options->colors.TaxiBack = { 0, 0, 0 };

	options->colors.WindowFont = { 0, 0, 255 };
	options->colors.WindowBack = { 0, 0, 0 };

	options->colors.BrickFont = { 188, 74, 60 };
	options->colors.BrickBack = { 0, 0, 0 };

	options->colors.GrassHomeFont = { 65, 152, 10 };
	options->colors.GrassHomeBack = { 0, 0, 0 };

	options->colors.BadCarFont = { 255, 0, 0 };
	options->colors.BadCarBack = { 0, 0, 0 };

	options->colors.FriendlyCarFont = { 0, 255, 0 };
	options->colors.FriendlyCarBack = { 0, 0, 0 };

	options->colors.StorkFont = { 255, 0, 0 };
	options->colors.StorkBack = { 255, 255, 255 };

	options->colors.CarLightFont = { 255, 255, 0 };
	options->colors.CarLightBack = { 0, 0, 0 };

	options->colors.BuildingFont = { 188, 74, 60 };
	options->colors.BuildingBack = { 65, 152, 10 };
	return options;
}

bool StartsWith(const char* str, const char* begin)
{
	size_t lenS = strlen(str);
	size_t lenBegin = strlen(begin);
	size_t maxLen = lenS < lenBegin ? lenS : lenBegin;
	for (size_t i = 0; i < maxLen; i++)
	{
		if (str[i] != begin[i])
		{
			return false;
		}
	}

	return true;
}

void ReadIntOption(char* buffer, const char* name, int& var)
{
	if (StartsWith(buffer, name))
	{
		char* pos = strchr(buffer, '=');
		if (pos != NULL)
		{
			var = atoi(pos + 1);
		}
	}
}

void ReadStringOption(char* buffer, const char* name, char*& var)
{
	if (StartsWith(buffer, name))
	{
		char* pos = strchr(buffer, '=');
		if (pos != NULL)
		{
			pos++;
			size_t len = strlen(pos);
			if (pos[len - 1] == '\n')
			{
				pos[len - 1] = '\0';
				len--;
			}

			delete var;
			var = new char[len];
			strcpy(var, pos);
		}
	}
}

void ReadFloatOption(char* buffer, const char* name, float& var)
{
	if (StartsWith(buffer, name))
	{
		char* pos = strchr(buffer, '=');
		if (pos != NULL)
		{
			var = atof(pos + 1);
		}
	}
}

void ReadBoolOption(char* buffer, const char* name, bool& var)
{
	if (StartsWith(buffer, name))
	{
		char* pos = strchr(buffer, '=');
		if (pos != NULL)
		{
			if (*(pos + 1) == 'y')
			{
				var = true;
			}
			else
			{
				var = false;
			}
		}
	}
}

void ReadRgbOption(char* buffer, const char* name, RGB& var)
{
	if (StartsWith(buffer, name))
	{
		char* pos = strchr(buffer, '=');
		if (pos != NULL)
		{
			var.r = strtol(pos + 1, NULL, 16);
			var.g = strtol(pos + 4, NULL, 16);
			var.b = strtol(pos + 7, NULL, 16);
		}
	}
}

void ReadCarOptions(char buffer[], Options* options)
{
	ReadIntOption(buffer, "car.speedUpFactor", options->car.speedUpFactor);
	ReadIntOption(buffer, "car.speedUpChances", options->car.speedUpChances);
	ReadIntOption(buffer, "car.slowDownChances", options->car.slowDownChances);
	ReadIntOption(buffer, "car.returnChances", options->car.returnChances);
	ReadIntOption(buffer, "car.wrapChances", options->car.wrapChances);
	ReadIntOption(buffer, "car.breakDistance", options->car.breakDistance);
	ReadIntOption(buffer, "car.carsNumber", options->car.carsNumber);
	ReadIntOption(buffer, "car.minSpeed", options->car.minSpeed);
	ReadIntOption(buffer, "car.maxSpeed", options->car.maxSpeed);
	ReadStringOption(buffer, "car.bikeSkin", options->car.bikeSkin);
	ReadStringOption(buffer, "car.carSkin", options->car.carSkin);
	ReadStringOption(buffer, "car.truckSkin", options->car.truckSkin);
}

void ReadGeneralOptions(char buffer[], Options* options)
{
	ReadIntOption(buffer, "general.startScreenWidth", options->general.startScreenWidth);
	ReadIntOption(buffer, "general.startScreenHeight", options->general.startScreenHeight);
	ReadIntOption(buffer, "general.gameOverScreenWidth", options->general.gameOverScreenWidth);
	ReadIntOption(buffer, "general.gameOverScreenHeight", options->general.gameOverScreenHeight);
	ReadIntOption(buffer, "general.maxTime", options->general.maxTime);
}

void ReadStorkOptions(char buffer[], Options* options)
{
	ReadIntOption(buffer, "stork.startX", options->stork.startX);
	ReadIntOption(buffer, "stork.startY", options->stork.startY);

	ReadFloatOption(buffer, "stork.speed", options->stork.speed);
	ReadStringOption(buffer, "stork.skin", options->stork.skin);
}

void ReadColorsOptions1(char buffer[], Options* options)
{
	ReadRgbOption(buffer, "colors.GrassFont", options->colors.GrassFont);
	ReadRgbOption(buffer, "colors.GrassBack", options->colors.GrassBack);

	ReadRgbOption(buffer, "colors.FrogFont", options->colors.FrogFont);
	ReadRgbOption(buffer, "colors.FrogBack", options->colors.FrogBack);

	ReadRgbOption(buffer, "colors.RoadFont", options->colors.RoadFont);
	ReadRgbOption(buffer, "colors.RoadBack", options->colors.RoadBack);

	ReadRgbOption(buffer, "colors.HomeFont", options->colors.HomeFont);
	ReadRgbOption(buffer, "colors.HomeBack", options->colors.HomeBack);

	ReadRgbOption(buffer, "colors.TaxiFont", options->colors.TaxiFont);
	ReadRgbOption(buffer, "colors.TaxiBack", options->colors.TaxiBack);
}

void ReadColorsOptions2(char buffer[], Options* options)
{

	ReadRgbOption(buffer, "colors.FrogBloodFontFont", options->colors.FrogBloodFontFont);
	ReadRgbOption(buffer, "colors.FrogBloodFontBack", options->colors.FrogBloodFontBack);

	ReadRgbOption(buffer, "colors.BloodFont", options->colors.BloodFont);
	ReadRgbOption(buffer, "colors.BloodBack", options->colors.BloodBack);

	ReadRgbOption(buffer, "colors.WindowFont", options->colors.WindowFont);
	ReadRgbOption(buffer, "colors.WindowBack", options->colors.WindowBack);
}

void ReadColorsOptions3(char buffer[], Options* options)
{
	ReadRgbOption(buffer, "colors.BrickFont", options->colors.BrickFont);
	ReadRgbOption(buffer, "colors.BrickBack", options->colors.BrickBack);

	ReadRgbOption(buffer, "colors.GrassHomeFont", options->colors.GrassHomeFont);
	ReadRgbOption(buffer, "colors.GrassHomeBack", options->colors.GrassHomeBack);

	ReadRgbOption(buffer, "colors.BadCarFont", options->colors.BadCarFont);
	ReadRgbOption(buffer, "colors.BadCarBack", options->colors.BadCarBack);

	ReadRgbOption(buffer, "colors.FriendlyCarFont", options->colors.FriendlyCarFont);
	ReadRgbOption(buffer, "colors.FriendlyCarBack", options->colors.FriendlyCarBack);

	ReadRgbOption(buffer, "colors.StorkFont", options->colors.StorkFont);
	ReadRgbOption(buffer, "colors.StorkBack", options->colors.StorkBack);

	ReadRgbOption(buffer, "colors.CarLightFont", options->colors.CarLightFont);
	ReadRgbOption(buffer, "colors.CarLightBack", options->colors.CarLightBack);

	ReadRgbOption(buffer, "colors.BuildingFont", options->colors.BuildingFont);
	ReadRgbOption(buffer, "colors.BuildingBack", options->colors.BuildingBack);
}

Options* ReadOptions(Options* options, const char* fileName)
{
	FILE* file = fopen(fileName, "r");
	if (file != NULL)
	{
		const int buffSize = 1024;
		char buff[buffSize];
		while (fgets(buff, buffSize - 1, file))
		{
			ReadGeneralOptions(buff, options);

			ReadStringOption(buff, "frog.skinOne", options->frog.skinOne);
			ReadStringOption(buff, "frog.skinTwo", options->frog.skinTwo);
			ReadIntOption(buff, "frog.jumpTime", options->frog.jumpTime);

			ReadCarOptions(buff, options);

			ReadIntOption(buff, "road.roadNumber", options->road.roadNumber);

			ReadStringOption(buff, "building.skin", options->building.skin);
			ReadIntOption(buff, "building.buildingsNumber", options->building.buildingsNumber);

			ReadStorkOptions(buff, options);
			
			ReadIntOption(buff, "board.width", options->board.width);

			ReadBoolOption(buff, "useSeed", options->useSeed);

			ReadIntOption(buff, "seed", options->seed);
			
			ReadStringOption(buff, "home.skin", options->home.skin);

			ReadStringOption(buff, "files.bestScoresFileName", options->files.bestScoresFileName);
			ReadStringOption(buff, "files.self", options->files.self);

			ReadColorsOptions1(buff, options);
			ReadColorsOptions2(buff, options);
			ReadColorsOptions3(buff, options);
		}

		fclose(file);
	}

	return options;
}

int InitSRand(Options* options)
{
	int seed = options->useSeed ? options->seed : time(NULL);
	srand(seed); // NOLINT(cert-msc51-cpp, clang-diagnostic-shorten-64-to-32)

	return seed;
}

// main

bool NextLevelExists(GameOverMessageData* message)
{
	int levelInt = message->levelId;
	if (levelInt == 0)
	{
		return false;
	}

	char fileName[30];
	sprintf(fileName, "Level%d.config", levelInt+1);

	int result = access(fileName, 0);
	return result == 0;
}

void NextLevelCreate(Options*& options, GameStateChange& change, GameOverMessageData* message)
{
	int levelInt = message->levelId;

	char fileName[30];
	sprintf(fileName, "Level%d.config", levelInt + 1);

	delete options;
	options = ReadOptions(CreateOptions(), fileName);
	options->seed = InitSRand(options);

	ChangeLevelData* level = new ChangeLevelData;
	level->score = message->score;
	level->levelId = levelInt + 1;
	change.data = level;
}

void ChangeToLevel1(Options*& options, GameStateChange change)
{
	ChangeLevelData* level = (ChangeLevelData*)change.data;
	int levelInt = level->levelId;

	char fileName[30];
	sprintf(fileName, "Level%d.config", levelInt);

	delete options;
	options = ReadOptions(CreateOptions(), fileName);
	options->seed = InitSRand(options);
}

void InitGameStates(GameState& start, GameState& game, GameState& gameOver, Options* options)
{
	start = { &StartInit, &StartKeysHandler, &StartTimerHandler, &StartDraw, &StartDone, options, NULL, false };
	game = { &GameInit, &GameKeysHandler, &GameTimerHandler, &GameDraw, &GameDone, options, NULL, true };
	gameOver = { &GameOverInit, &GameOverKeysHandler, &GameOverTimerHandler, &GameOverDraw, &GameOverDone, options, NULL, false };
}

void StartReplay(Options*& options, GameState game, Recorder* rec, GameState& current)
{
	delete options;
	options = ReadOptions(CreateOptions(), rec->optionsName);
	options->seed = rec->seed;
	options->useSeed = true;
	options->seed = InitSRand(options);
	current = game;
}

int main()
{
	Options* ops = ReadOptions(CreateOptions(), "frog.config");

	ops->seed = InitSRand(ops);

	WINDOW* win = InitWindow(ops->colors);
	GameState start;
	GameState game;
	GameState gameOver;
	InitGameStates(start, game, gameOver, ops);

	Recorder* rec = NULL;

	GameState current = start;
	current.init(current, ops, NULL);
	
	while (true)
	{
		GameStateChange change = MainLoop(current, ops, rec, win);
		if (rec != NULL)
		{
			delete rec;
			rec = NULL;
		}
		
		current.done(current, ops, current.data);
		switch (change.message)
		{
			case ChangeToStart:
			{
				current = start;
				break;
			}
			case ChangeToGame:
			{
				delete ops;
				ops = ReadOptions(CreateOptions(), "frog.config");
				ops->seed = InitSRand(ops);
				current = game;
				break;
			}
			case ChangeToGameOver:
			{
				GameOverMessageData* message = (GameOverMessageData*)change.data;
				
				if (message->won && NextLevelExists(message))
				{
					NextLevelCreate(ops, change, message);
					break;
				}

				current = gameOver;
				break;
			}
			case ExitProgram:
			{
				return 0;
			}
			case ChangeNoChange:
			{
				break;
			}
			case ChangeToLevel:
			{
				ChangeToLevel1(ops, change);
				current = game;
				break;
			}
			case ChangeToReplay:
			{
				rec = LoadRec();
				StartReplay(ops, game, rec, current);
				break;
			}
		}
		current.init(current, ops, change.data);
	}
}
