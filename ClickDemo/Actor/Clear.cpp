#include "Clear.h"
#include "Engine/Engine.h"
#include "Level/GameLevel.h"

Clear::Clear(const Vector2& position, GameLevel* level)
	: DrawableActor("G"), refLevel(level)
{
	this->position = position;
	color = Color::Yellow;
}

void Clear::Update(float deltaTime)
{
	Super::Update(deltaTime);
}
