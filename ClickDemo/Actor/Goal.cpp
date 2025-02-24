#include "Goal.h"
#include "Engine/Engine.h"

Goal::Goal(const Vector2& position)
	: DrawableActor("e")
{
	this->position = position;
	color = Color::Green;
}

void Goal::Update(float deltaTime)
{
	DrawableActor::Update(deltaTime);

	if (Engine::Get().GetKeyDown(VK_ESCAPE))
	{
		Engine::Get().QuitGame();
	}

	if (Engine::Get().GetKeyDown(VK_RBUTTON))
	{
		position = Engine::Get().MousePosition();
	}
}