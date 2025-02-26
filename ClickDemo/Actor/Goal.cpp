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

#pragma region �̵�/����
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
#pragma endregion ����Ű or ��Ŭ��

	auto map = refLevel->GetMap();
	Actor* actor = map[position.y][position.x];

	// Enemy�� ������ ���� ����
	if (dynamic_cast<Start*>(actor))
	{
		bIsOver = true;
	}
	
	// Clear(��ǥ����)�� �����ϸ� ���� ����
	if (dynamic_cast<Clear*>(actor))
	{
		bIsClear = true;
	}

}

void Goal::CanMove(Vector2 newPosition)
{
	auto map = refLevel->GetMap();
	Actor* actor = map[newPosition.y][newPosition.x];

	// �̵��� ��ġ�� ���� ����ų� Ground�� �ƴ϶�� ����
	if (newPosition.y >= map.size()
		|| newPosition.x >= map[0].size()
		|| dynamic_cast<Wall*>(actor)
		|| dynamic_cast<Start*>(actor)) return;

	// ���� ��ġ�� Ground�� ����
	map[position.y][position.x] = new Ground(position);

	// ���ο� ��ġ�� ����
	position = newPosition;
	map[position.y][position.x] = this;

	bIsResetGoal = true;
	refLevel->SetRestartingPathFindingFlag(bIsResetGoal);
}