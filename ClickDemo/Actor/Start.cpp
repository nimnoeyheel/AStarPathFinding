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

		// 클릭한 위치가 맵을 벗어났을 경우 리턴
		if (newPosition.y > map.size()
			|| newPosition.x > map[0].size())
		{
			return;
		}

		Actor* actor = map[newPosition.y][newPosition.x];

		// 클릭한 위치가 Ground일 경우에만 위치 갱신
		if (dynamic_cast<Ground*>(actor))
		{
			// 기존 위치를 Ground로 변경
			map[position.y][position.x] = new Ground(position);

			// 새로운 위치로 갱신
			position = newPosition;
			map[position.y][position.x] = this;

			refLevel->FindPath();
		}
		else return;
	}
}