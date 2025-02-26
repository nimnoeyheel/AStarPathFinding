#include "Goal.h"
#include "Engine/Engine.h"
#include "Level/GameLevel.h"
#include "Ground.h"
#include "Wall.h"
#include "Start.h"
#include "Clear.h"

Goal::Goal(const Vector2& position, GameLevel* level)
	: DrawableActor("P"), refLevel(level)
{
	this->position = position;
	color = Color::Green;
}

void Goal::Update(float deltaTime)
{
	DrawableActor::Update(deltaTime);

#pragma region 이동/종료
	if (Engine::Get().GetKeyDown(VK_ESCAPE))
	{
		Engine::Get().QuitGame();
	}

	if (Engine::Get().GetKeyDown(VK_LEFT))
	{
		Vector2 newPosition(position.x - 1, position.y);
		CanMove(newPosition);
	}
	if (Engine::Get().GetKeyDown(VK_RIGHT))
	{
		Vector2 newPosition(position.x + 1, position.y);
		CanMove(newPosition);
	}
	if (Engine::Get().GetKeyDown(VK_DOWN))
	{
		Vector2 newPosition(position.x, position.y + 1);
		CanMove(newPosition);
	}
	if (Engine::Get().GetKeyDown(VK_UP))
	{
		Vector2 newPosition(position.x, position.y - 1);
		CanMove(newPosition);
	}

	if (Engine::Get().GetKeyDown(VK_RBUTTON))
	{
		Vector2 newPosition = Vector2(Engine::Get().MousePosition());
		CanMove(newPosition);
	}
#pragma endregion 방향키 or 좌클릭

	auto map = refLevel->GetMap();
	Actor* actor = map[position.y][position.x];

	// Enemy와 닿으면 게임 종료
	if (dynamic_cast<Start*>(actor))
	{
		bIsOver = true;
	}
	
	// Clear(목표지점)에 도달하면 게임 종료
	if (dynamic_cast<Clear*>(actor))
	{
		bIsClear = true;
	}

}

void Goal::CanMove(Vector2 newPosition)
{
	auto map = refLevel->GetMap();
	Actor* actor = map[newPosition.y][newPosition.x];

	// 이동할 위치가 맵을 벗어났거나 Ground가 아니라면 리턴
	if (newPosition.y >= map.size()
		|| newPosition.x >= map[0].size()
		|| dynamic_cast<Wall*>(actor)
		|| dynamic_cast<Start*>(actor)) return;

	// 기존 위치를 Ground로 변경
	map[position.y][position.x] = new Ground(position);

	// 새로운 위치로 갱신
	position = newPosition;
	map[position.y][position.x] = this;

	bIsResetGoal = true;
	refLevel->SetRestartingPathFindingFlag(bIsResetGoal);
}