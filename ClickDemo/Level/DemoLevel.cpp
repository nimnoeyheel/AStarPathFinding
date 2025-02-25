#include "DemoLevel.h"
#include "Actor/Goal.h"
#include "Actor/Start.h"
#include "Actor/Ground.h"
#include "Actor/Wall.h"
#include <string>
#include <fstream>
#include <Engine/Engine.h>
#include "Algorithm/AStar.h"
#include "Algorithm/Node.h"
#include "Actor/Path.h"

DemoLevel::DemoLevel()
	: /*astar(new AStar(this)),*/ lastUpdateTime(0.0f), pathIndex(0), drawTimer(0.25f)
{
	// 맵 파일 불러오기
	ParseMap("../Assets/Map2.txt");

}

void DemoLevel::ParseMap(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cerr << "파일을 읽어올 수 없습니다.\n";
	}

	map.clear();

	int y = 0;
	std::string line;

	// 한 줄씩 읽어서 처리
	while (std::getline(file, line))
	{
		std::vector<Actor*> mapData;
		for (int x = 0; x < line.size(); ++x)
		{
			char mapChar = line[x];
			if (mapChar == '.') // Ground
			{
				mapData.emplace_back(new Ground(Vector2(x, y)));
			}
			else if (mapChar == '1') // Wall
			{
				mapData.emplace_back(new Wall(Vector2(x, y)));
			}
			else if (mapChar == 's') // Start
			{
				start = new Start(Vector2(x, y), this);
				mapData.emplace_back(start);
			}
			else if (mapChar == 'e') // Goal
			{
				goal = new Goal(Vector2(x, y), this);
				mapData.emplace_back(goal);
			}
			else continue;
		}
		map.emplace_back(mapData);
		++y;
	}

	file.close();
}

void DemoLevel::Update(float deltaTime)
{
	if (start && goal)
	{
		start->Update(deltaTime);
		goal->Update(deltaTime);
	}

	// 0.5초마다 실행
	drawTimer.Update(deltaTime);

	if (drawTimer.IsTimeOut())
	{
		drawTimer.Reset();

		if (pathIndex < path.size())
		{
			// 노드 한개씩 가져오기
			Node* currentNode = path[pathIndex];
			Vector2 pos = currentNode->Position();

			// 기존 Ground 자리에 Path 객체 삽입
			map[pos.y][pos.x] = new Path(pos);

			++pathIndex;
		}

		Draw();
	}
	/*// 1초마다 경로의 다음 노드를 렌더링
	lastUpdateTime += deltaTime;
	if (lastUpdateTime >= 1.0f && pathIndex < path.size())
	{
		lastUpdateTime = 0.0f;
		
		// 경로에서 한 개씩 노드를 가져와서 표시
		Node* currentNode = path[pathIndex];
		Vector2 pos = currentNode->Position();

		// 기존 타일을 제거하고 Path 객체 삽입
		delete map[pos.y][pos.x];
		map[pos.y][pos.x] = new Path(pos);

		// 다음 경로로 이동
		++pathIndex;

		Draw();
	}*/
}

void DemoLevel::Draw()
{
	for (auto line : map)
	{
		for (auto data : line)
		{
			data->Draw();
		}
	}
}

void DemoLevel::FindPath()
{
	if (!start || !goal) return;

	// 그리드 생성
	int mapY = (int)map.size();
	int mapX = (int)map[0].size();
	std::vector<std::vector<int>> grid(mapY, std::vector<int>(mapX, 0));
	for (int y = 0; y < mapY; ++y)
	{
		for (int x = 0; x < mapX; ++x)
		{
			if (dynamic_cast<Wall*>(map[y][x]))
			{
				grid[y][x] = 1; // 장애물
			}
		}
	}

	// A* 알고리즘 실행
	AStar* astar = new AStar();
	Node startNode(start->Position());
	Node goalNode(goal->Position());

	path = astar->FindPath(&startNode, &goalNode, grid);

	// 초기 경로 인덱스 설정
	pathIndex = 0;
}