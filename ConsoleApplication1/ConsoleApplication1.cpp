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
	GrassPair = 1,
	FrogPair = 2,
	RoadPair = 3,
	HomePair = 4,
	TaxiFontPair = 5,
	FrogBloodFontPair = 6,
	BloodPair = 7,
	WindowPair = 8,
	BrickPair = 9,
	GrassFontPair = 10,
	BadCarPair = 11,
	FriendlyCarPair = 12,
	StorkPair = 13,
	CarLightsPair = 14,
	BuildingPair = 15,
};

enum Colors
{
	// TODO: nie powinnismy uzywac predefiniwoanych kolorow jak WHITE czy YELLOW
	GrassFontColor = 1,
	GrassBackColor = 2,
	FrogFontColor = 3,
	FrogBackColor = 4,
	RoadFontColor = 5,
	RoadBackColor = 6,
	BrickColor = 7,
	FrogBloodFontColor = 8,
	FrogBloodBackColor = 9,
	HomeFontColor = 10,
	HomeBackColor = 11,
	WindowFontColor = 12,
	WindowBackColor = 13,
	TaxiFontColor = 14,
	TaxiBackColor = 15,
	BackgroundColor = 16,
	BadCarFontColor = 17,
	BadCarBackColor = 18,
	FriendlyCarFontColor = 19,
	FriendlyCarBackColor = 20,
	StorkFontColor = 21,
	StorkBackColor = 22,
	CarLightFontColor = 23,
	CarLightBackColor = 24,
	BuildingFontColor = 25,
	BuildingBackColor = 26,
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

// options

struct HomeOptions
{
	char skin[2];
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

	char bikeSkin[4];
	char carSkin[4];
	char truckSkin[4];
};

struct FrogOptions
{
	char skinOne[2];
	char skinTwo[2];
};

struct StorkOptions
{
	int startX, startY;
	float speed;
	char skin[2];
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
	// TODO: tu dodac parametry dla kolorow
	RGB Background;
	RGB GrassFont, GrassBack;
	RGB FrogFont, FrogBack;
	RGB RoadFont, RoadBack;
	RGB HomeFont, HomeBack;
	RGB TaxiFont, TaxiBack;
	RGB FrogBloodFont, FrogBloodBack;
	RGB WindowFont, WindowBack;
	RGB BrickFont, BrickBack;
	RGB BadCarFont, BadCarBack;
	RGB FriendlyCarFont, FriendlyCarBack;
	RGB StorkFont, StorkBack;
	RGB LightFont, LightBack;
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
	int roadsNumber;
	int width;
	int score;
	int time; // in miliseconds
	int maxTime;// in seconds
	Car* cars;
	int carsNumber;
	Building* buildings;
	int buildingsNumber;
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

WINDOW* InitWindow(const ColorsOptions& colors)
{
	// curses init
	WINDOW*win = initscr();
	curs_set(0);

	// colors init
	if (has_colors())
	{
		start_color();

		// TODO: tu pobierac kolory z parametrow
		InitColor(BackgroundColor, colors.Background.r, colors.Background.g, colors.Background.b);
		InitColor(BrickColor, colors.BrickFont.r, colors.BrickFont.g, colors.BrickFont.b);

		InitColor(CarLightFontColor, colors.LightFont.r, colors.LightFont.g, colors.LightFont.b);
		InitColor(CarLightBackColor, colors.LightBack.r, colors.LightBack.g, colors.LightBack.b);

		InitColor(GrassFontColor, colors.GrassFont.r, colors.GrassFont.g, colors.GrassFont.b);
		InitColor(GrassBackColor, colors.GrassBack.r, colors.GrassBack.g, colors.GrassBack.b);

		InitColor(FrogFontColor, colors.FrogFont.r, colors.FrogFont.g, colors.FrogFont.b);
		InitColor(FrogBackColor, colors.FrogBack.r, colors.FrogBack.g, colors.FrogBack.b);

		InitColor(RoadFontColor, colors.RoadFont.r, colors.RoadFont.g, colors.RoadFont.b);
		InitColor(RoadBackColor, colors.RoadBack.r, colors.RoadBack.g, colors.RoadBack.b);

		InitColor(HomeFontColor, colors.HomeFont.r, colors.HomeFont.g, colors.HomeFont.b);
		InitColor(HomeBackColor, colors.HomeBack.r, colors.HomeBack.g, colors.HomeBack.b);

		InitColor(HomeFontColor, colors.FrogBloodFont.r, colors.FrogBloodFont.g, colors.FrogBloodFont.b);
		InitColor(HomeBackColor, colors.FrogBloodBack.r, colors.FrogBloodBack.g, colors.FrogBloodBack.b);

		InitColor(WindowFontColor, colors.WindowFont.r, colors.WindowFont.g, colors.WindowFont.b);
		InitColor(WindowBackColor, colors.WindowBack.r, colors.WindowBack.g, colors.WindowBack.b);

		InitColor(TaxiFontColor, colors.TaxiFont.r, colors.TaxiFont.g, colors.TaxiFont.b);
		InitColor(TaxiBackColor, colors.TaxiBack.r, colors.TaxiBack.g, colors.TaxiBack.b);

		InitColor(BadCarFontColor, colors.BadCarFont.r, colors.BadCarFont.g, colors.BadCarFont.b);
		InitColor(BadCarBackColor, colors.BadCarBack.r, colors.BadCarBack.g, colors.BadCarBack.b);

		InitColor(FriendlyCarFontColor, colors.FriendlyCarFont.r, colors.FriendlyCarFont.g, colors.FriendlyCarFont.b);
		InitColor(FriendlyCarBackColor, colors.FriendlyCarBack.r, colors.FriendlyCarBack.g, colors.FriendlyCarBack.b);

		InitColor(StorkFontColor, colors.StorkFont.r, colors.StorkFont.g, colors.StorkFont.b);
		InitColor(StorkBackColor, colors.StorkBack.r, colors.StorkBack.g, colors.StorkBack.b);

		InitColor(BuildingFontColor, colors.BuildingFont.r, colors.BuildingFont.g, colors.BuildingFont.b);
		InitColor(BuildingBackColor, colors.BuildingBack.r, colors.BuildingBack.g, colors.BuildingBack.b);


		InitColorPair(GrassPair, GrassFontColor, GrassBackColor);
		InitColorPair(FrogPair, FrogFontColor, FrogBackColor);
		InitColorPair(RoadPair, RoadFontColor, RoadBackColor);
		InitColorPair(HomePair, HomeFontColor, HomeBackColor);
		InitColorPair(TaxiFontPair, TaxiFontColor, TaxiBackColor);
		InitColorPair(FrogBloodFontPair, FrogBloodFontColor, BackgroundColor);
		InitColorPair(BloodPair, FrogBloodFontColor, FrogBloodBackColor);
		InitColorPair(WindowPair, WindowFontColor, WindowBackColor);
		InitColorPair(FriendlyCarPair, FriendlyCarFontColor, FriendlyCarBackColor);
		InitColorPair(BadCarPair, BadCarFontColor, BadCarBackColor);
		InitColorPair(BrickPair, BrickColor, BackgroundColor);
		InitColorPair(GrassFontPair, GrassFontColor, BackgroundColor);
		InitColorPair(StorkPair, StorkFontColor, StorkBackColor);
		InitColorPair(CarLightsPair, CarLightFontColor, CarLightBackColor);
		InitColorPair(BuildingPair, BuildingFontColor, BuildingBackColor);
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

GameState CreateStart(Options* options)
{
	GameState start;
	start.options = options;
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

void ChangeCarRoad(const Board* b, Car& c)
{
	int numberOfStreets = GetNumberOfStreets(b, Street);
	int* streets = GetIndexesOfStreets(b, numberOfStreets, Street);

	c.x = 1.0f - c.size;
	int newRoad = rand() % numberOfStreets;
	c.roadNumber = streets[newRoad];

	delete[] streets;
}

GameStateChange MoveCars(Board* b, const Options* options, int deltaTime)
{
	for (int i = 0; i < b->carsNumber; ++i)
	{
		Car& c = b->cars[i];
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

				c.x -= deltaTime * c.speed / 1000.0f;

				if (c.x <= 0)
				{
					int wrap = rand() % 100;

					if (wrap > options->car.wrapChances)
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
				if (c.type == Friendly && 0 <= distance && distance <= options->car.breakDistance && c.roadNumber == b->frog.y)
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
					int wrap = rand() % 100;

					if (wrap > options->car.wrapChances)
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
			StartPair(TaxiFontPair);
			printw(carsChars[c.size]);
			EndPair(TaxiFontPair);
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
	printw("Time left: %ds    Score: %d\n", board->maxTime - board->time/1000, board->score);

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

void InitFrog(Board* board)
{
	int x = rand() % board->width / 2;
	board->frog = { x, board->roadsNumber - 1, 0, NULL, false };
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
	board->score = 0;
	board->time = 0;
	board->maxTime = options->general.maxTime;
}

void InitStork(Board* board, const Options* options)
{
	board->stork = { options->stork.startX, options->stork.startY, 0.0f, options->stork.speed };
}

void GameInit(GameState& self, const Options* options, void* initData)
{
	Board* board = new Board;
	board->width = options->board.width;

	InitRoads(board, options);

	InitFrog(board);

	InitCars(board, options);

	InitBuildings(board, options);

	InitOtherVariables(board, options);

	InitStork(board, options);

	int cols = board->width + 1;
	int lines = board->roadsNumber + 2;

	Resize(cols, lines);

	self.data = board;
}

GameState CreateGame(Options* options)
{
	GameState game;
	game.options = options;
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
	StartPair(GrassFontPair); DrawStr("~", 55); EndPair(GrassFontPair);
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

GameState CreateGameOver(Options* options)
{
	GameState gameOver;
	gameOver.options = options;
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

GameStateChange MainLoop(const GameState& current, const Options* options, WINDOW* win)
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
		change = current.timerHandler(current, options, time);
		if (change.message != ChangeNoChange)
		{
			return change;
		}

		current.draw(current, options, win);
	}
}

Options* CreateOptions()
{
	Options* options = new Options;
	options->general.startScreenWidth = 53;
	options->general.startScreenHeight = 23;
	options->general.gameOverScreenWidth = 56;
	options->general.gameOverScreenHeight = 45;
	options->general.maxTime = 100;

	options->frog.skinOne[0] = 'F';
	options->frog.skinOne[1] = '\0';
	options->frog.skinTwo[0] = 'f';
	options->frog.skinTwo[1] = '\0';

	options->home.skin[0] = 'H';
	options->home.skin[1] = '\0';

	options->car.speedUpFactor = 5.0f;
	options->car.speedUpChances = 10;
	options->car.slowDownChances = 10;
	options->car.returnChances = 10;
	options->car.wrapChances = 50;
	options->car.breakDistance = 2;
	options->car.carsNumber = 10;
	options->car.minSpeed = 5;
	options->car.maxSpeed = 15;

	options->car.bikeSkin[0] = '#';
	options->car.bikeSkin[1] = '\0';

	options->car.carSkin[0] = '#';
	options->car.carSkin[1] = '#';
	options->car.carSkin[2] = '\0';

	options->car.truckSkin[0] = '#';
	options->car.truckSkin[1] = '#';
	options->car.truckSkin[2] = '#';
	options->car.truckSkin[3] = '\0';

	options->road.roadNumber = 10;

	options->building.buildingsNumber = 10;

	options->building.skin = new char[2];
	strcpy(options->building.skin, "A");

	options->stork.startX = 0;
	options->stork.startY = 0;
	options->stork.speed = 1.0f;
	options->stork.skin[0] = 'S';
	options->stork.skin[1] = '\0';

	options->board.width = 80;

	options->useSeed = false;
	options->seed = 1;

	options->files.bestScoresFileName = new char[9];
	strcpy(options->files.bestScoresFileName, "best.txt");

	// TODO: tu dodac domyslne wartosci kolorow

	// default colors

	options->colors.Background = { 0, 0, 0 };
	options->colors.BrickFont = { 0, 0, 0 };
	options->colors.BrickBack = { 0, 0, 0 };

	options->colors.GrassFont = { 65,152,10 };
	options->colors.GrassBack = { 65,152,10 };

	options->colors.FrogFont = { 0, 0, 0 };
	options->colors.FrogBack = { 0,0,0 };

	options->colors.RoadFont = { 0, 0, 0 };
	options->colors.RoadBack = { 0, 0, 0 };

	options->colors.FrogBloodFont = { 0, 0, 0 };
	options->colors.FrogBloodBack = { 0, 0, 0 };

	options->colors.HomeFont = { 10, 10, 10 };
	options->colors.HomeBack = { 10, 10, 10 };
	/*
	options->colors.Background = { 0, 0, 0 };
	options->colors.BrickFont = { 188, 74, 60};
	options->colors.BrickBack = { 0, 0, 0};

	options->colors.GrassFont = { 65,152,10 };
	options->colors.GrassBack = { 65,152,10 };

	options->colors.FrogFont = { 153, 198, 142 };
	options->colors.FrogBack = { 0,0,0 };

	options->colors.RoadFont = { 179, 179, 179 };
	options->colors.RoadBack = { 179, 179, 179 };

	options->colors.FrogBloodFont = { 120, 6, 6 };
	options->colors.FrogBloodBack = { 120, 6, 6 };

	options->colors.HomeFont = { 188, 74, 60 };
	options->colors.HomeBack = { 65, 152, 10 };
	*/

	options->colors.TaxiFont = { 0, 0, 0 };
	options->colors.TaxiBack = { 0, 0, 0 };

	options->colors.WindowFont = { 0, 0, 0 };
	options->colors.WindowBack = { 0, 0, 0 };

	options->colors.BadCarFont = { 0, 0, 0 };
	options->colors.BadCarBack = { 0, 0, 0 };

	options->colors.FriendlyCarFont = { 0, 0, 0 };
	options->colors.FriendlyCarBack = { 0, 0, 0 };

	options->colors.StorkFont = { 0, 0, 0 };
	options->colors.StorkBack = { 0, 0, 0 };

	options->colors.LightFont = { 0, 0, 0 };
	options->colors.LightBack = { 0, 0, 0 };

	options->colors.BuildingFont = { 10, 10, 10 };
	options->colors.BuildingBack = { 10, 10, 10 };

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

// TODO: do usuniecia po przerobieniu odczytu parametrow
int GetInt(char* str)
{
	char* pos = strchr(str, '=');
	if (pos == NULL)
	{
		return 0;
	}

	int result = atoi(pos + 1);
	return result;
}

// TODO: do usuniecia po przerobieniu odczytu parametrow
char* GetString(char* str)
{
	char* pos = strchr(str, '=');
	if (pos == NULL)
	{
		return NULL;
	}

	size_t len = strlen(pos);
	if (pos[len-1] == '\n')
	{
		pos[len - 1] = '\0';
	}

	return pos + 1;
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

Options* ReadOptions(Options* options)
{
	FILE* file = fopen("frog.config", "r");
	if (file != NULL)
	{
		const int bufferSize = 1024;
		char buffer[bufferSize];
		while (fgets(buffer, bufferSize - 1, file))
		{
			// TODO: paramsy
			ReadIntOption(buffer, "general.startScreenWidth", options->general.startScreenWidth);

			if (StartsWith(buffer, "general.startScreenHeight"))
			{
				options->general.startScreenHeight = GetInt(buffer);
			}
			else if (StartsWith(buffer, "general.gameOverScreenWidth"))
			{
				options->general.gameOverScreenWidth = GetInt(buffer);
			}
			else if (StartsWith(buffer, "general.gameOverScreenHeight"))
			{
				options->general.gameOverScreenHeight = GetInt(buffer);
			}
			else if (StartsWith(buffer, "general.maxTime"))
			{
				options->general.maxTime = GetInt(buffer);
			}
			else if (StartsWith(buffer, "frog.skinOne"))
			{
				options->frog.skinOne[0] = GetString(buffer)[0];
			}
			else if (StartsWith(buffer, "frog.skinTwo"))
			{
				options->frog.skinTwo[0] = GetString(buffer)[0];
			}
			else if (StartsWith(buffer, "car.speedUpFactor"))
			{
				options->car.speedUpFactor = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.speedUpChances"))
			{
				options->car.speedUpChances = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.slowDownChances"))
			{
				options->car.slowDownChances = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.returnChances"))
			{
				options->car.returnChances = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.wrapChances"))
			{
				options->car.wrapChances = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.breakDistance"))
			{
				options->car.breakDistance = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.carsNumber"))
			{
				options->car.carsNumber = GetInt(buffer);
			}
			else if (StartsWith(buffer, "road.roadNumber"))
			{
				options->road.roadNumber = GetInt(buffer);
			}
			else if (StartsWith(buffer, "building.buildingsNumber"))
			{
				options->building.buildingsNumber = GetInt(buffer);
			}
			else if (StartsWith(buffer, "stork.startX"))
			{
				options->stork.startX = GetInt(buffer);
			}
			else if (StartsWith(buffer, "stork.startY"))
			{
				options->stork.startY = GetInt(buffer);
			}
			else if (StartsWith(buffer, "stork.speed"))
			{
				options->stork.speed = GetInt(buffer);
			}
			else if (StartsWith(buffer, "board.width"))
			{
				options->board.width = GetInt(buffer);
			}
			else if (StartsWith(buffer, "useSeed"))
			{
				options->useSeed = GetString(buffer)[0] == 'y';
			}
			else if (StartsWith(buffer, "seed"))
			{
				options->seed = GetInt(buffer);
			}
			else if (StartsWith(buffer, "stork.skin"))
			{
				options->stork.skin[0] = GetString(buffer)[0];
			}
			else if (StartsWith(buffer, "home.skin"))
			{
				options->home.skin[0] = GetString(buffer)[0];
			}
			else if (StartsWith(buffer, "car.minSpeed"))
			{
				options->car.minSpeed = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.maxSpeed"))
			{
				options->car.maxSpeed = GetInt(buffer);
			}
			else if (StartsWith(buffer, "car.bikeSkin"))
			{
				options->car.bikeSkin[0] = GetString(buffer)[0];
			}
			else if (StartsWith(buffer, "car.carSkin"))
			{
				options->car.carSkin[0] = GetString(buffer)[0];
				options->car.carSkin[1] = GetString(buffer)[1];
			}
			else if (StartsWith(buffer, "car.truckSkin"))
			{
				options->car.truckSkin[0] = GetString(buffer)[0];
				options->car.truckSkin[1] = GetString(buffer)[1];
				options->car.truckSkin[2] = GetString(buffer)[2];
			}

			ReadStringOption(buffer, "building.skin", options->building.skin);
			ReadStringOption(buffer, "files.bestScoresFileName", options->files.bestScoresFileName);

			// TODO: tu dodac odczyt parametrow kolorow
			ReadRgbOption(buffer, "color.grass.font", options->colors.GrassFont);
		}

		fclose(file);
	}

	return options;
}

int main()
{
	Options* options = ReadOptions(CreateOptions());

	if (options->useSeed)
	{
		srand(options->seed);
	}
	else
	{
		srand(time(NULL)); // NOLINT(cert-msc51-cpp, clang-diagnostic-shorten-64-to-32)
	}

	WINDOW* win = InitWindow(options->colors);

	GameState Start = CreateStart(options);
	GameState Game = CreateGame(options);
	GameState GameOver = CreateGameOver(options);

	GameState current = Start;
	current.init(current, options, NULL);
	
	while (true)
	{
		GameStateChange change = MainLoop(current, options, win);
		current.done(current, options, current.data);
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
		current.init(current, options, change.data);
	}
}

// przy delete zrzutowac wskaznik przed delete
// wszystkie parametry umieœciæ w pliku