#include <stdio.h>

struct GameVariables
{
	int width, height;
	char** screen;
	bool run;
};

void PrintGameFrame(GameVariables game)
{
	printf("%d\n", game.height);
}

void GameFrame(GameVariables game)
{
	PrintGameFrame(game);
}

bool Init(GameVariables& game)
{
	game.width = 10;
	game.height = 10;
	game.run = true;
	return true;
}

int main()
{
	GameVariables game = {};

	if (!Init(game))
	{
		return -1;
	}

	while (game.run)
	{
		GameFrame(game);
	}

	return 0;
}