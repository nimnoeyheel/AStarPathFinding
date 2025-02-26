#include "Engine/Engine.h"
#include "Level/GameLevel.h"

int main()
{
	Engine engine;
	engine.LoadLevel(new GameLevel());
	engine.Run();
}