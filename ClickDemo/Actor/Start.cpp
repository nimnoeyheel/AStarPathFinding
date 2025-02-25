#include "Start.h"
#include "Engine/Engine.h"
#include "Level/DemoLevel.h"
#include "Ground.h"

Start::Start(const Vector2& position, DemoLevel* level)
	: DrawableActor("s") , refLevel(level)
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
		auto map = refLevel->GetMap();

		// Ŭ���� ��ġ�� ���� ����� ��� ����
		if (newPosition.y > map.size()
			|| newPosition.x > map[0].size())
		{
			return;
		}

		Actor* actor = map[newPosition.y][newPosition.x];

		// Ŭ���� ��ġ�� Ground�� ��쿡�� ��ġ ����
		if (dynamic_cast<Ground*>(actor))
		{
			// ���� ��ġ�� Ground�� ����
			map[position.y][position.x] = new Ground(position);

			// ���ο� ��ġ�� ����
			position = newPosition;
			map[position.y][position.x] = this;

			refLevel->FindPath();
		}
		else return;
	}
}