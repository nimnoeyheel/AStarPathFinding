#include "Start.h"
#include "Engine/Engine.h"
#include "Level/GameLevel.h"
#include "Ground.h"
#include "Wall.h"

Start::Start(const Vector2& position, GameLevel* level)
	: DrawableActor("E") , refLevel(level)
{
	this->position = position;
	color = Color::Red;
}

void Start::Update(float deltaTime)
{
	Super::Update(deltaTime);
	
	if (Engine::Get().GetKeyDown(VK_LBUTTON))
	{
		Vector2 newPosition = Engine::Get().MousePosition();
		CanMove(newPosition);
	}
}

void Start::CanMove(Vector2 newPosition)
{
	auto map = refLevel->GetMap();
	Actor* actor = map[newPosition.y][newPosition.x];

	// �̵��� ��ġ�� ���� ����ų� Ground�� �ƴ϶�� ����
	if (newPosition.y > map.size()
		|| newPosition.x > map[0].size()
		|| dynamic_cast<Wall*>(actor)) return;

	// ���� ��ġ�� Ground�� ����
	map[position.y][position.x] = new Ground(position);

	// ���ο� ��ġ�� ����
	position = newPosition;
	map[position.y][position.x] = this;

	bIsResetStart = true;

	refLevel->FindPath();
}
