#include "GameLevel.h"
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
#include "Actor/Clear.h"
#include "MenuLevel.h"

GameLevel::GameLevel()
	: lastUpdateTime(0.0f), pathIndex(0), drawTimer(0.3f)
{
	// 맵 파일 불러오기
	ParseMap("../Assets/Map2.txt");
}

void GameLevel::ParseMap(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open()) return;

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
			else if (mapChar == 's') // Start(Enemy)
			{
				start = new Start(Vector2(x, y), this);
				mapData.emplace_back(start);
			}
			else if (mapChar == 'e') // Goal(Plyer)
			{
				goal = new Goal(Vector2(x, y), this);
				mapData.emplace_back(goal);
			}
			else if (mapChar == 'c') // Clear
			{
				clear = new Clear(Vector2(x, y), this);
				mapData.emplace_back(clear);
			}
			else continue;
		}
		map.emplace_back(mapData);
		++y;
	}

	file.close();
}

void GameLevel::Update(float deltaTime)
{
	FindPath();

	if (start && goal)
	{
		start->Update(deltaTime);
		goal->Update(deltaTime);
	}

	// 게임 종료
	if (goal->IsClear())
	{
		Engine::Get().LoadLevel(new MenuLevel("Game Clear!!"));
	}
	else if (goal->IsOver())
	{
		Engine::Get().LoadLevel(new MenuLevel("Game Over!!"));
	}

	// Goal 위치가 갱신된 후, pathIndex의 마지막 부분에서부터 다시 탐색 시작
	if (bShouldRestartPathFinding)
	{
		FindPath();
		bShouldRestartPathFinding = false;
		return;
	}

	// 타이머 설정
	drawTimer.Update(deltaTime);
	if (drawTimer.IsTimeOut())
	{
		drawTimer.Reset();
		if (pathIndex < (int)path.size())
		{
			// 이전 Start 위치 저장
			Vector2 prevPosition = start->Position();

			// 노드 한 개씩 가져오기
			Node* currentNode = path[pathIndex];
			Vector2 pos = currentNode->Position();

			// 기존 Start 위치를 Ground로 변경
			map[prevPosition.y][prevPosition.x] = new Ground(prevPosition);

			// 현재 위치에 Start 삽입
			start->SetPosition(pos);
			map[pos.y][pos.x] = start;

			// 기존 Ground 자리에 Path 객체 삽입
			//map[pos.y][pos.x] = new Path(pos);

			++pathIndex;
		}
	}
}

void GameLevel::Draw()
{
	for (auto line : map)
	{
		for (auto data : line)
		{
			data->Draw();
		}
	}

	if (start && goal)
	{
		goal->Draw();
		start->Draw();
	}
}

void GameLevel::ClearPreviousPath()
{
	// 기존 Path를 모두 Ground로 변경
	for (int y = 0; y < (int)map.size(); ++y)
	{
		for (int x = 0; x < (int)map[0].size(); ++x)
		{
			if (dynamic_cast<Path*>(map[y][x]))
			{
				//delete map[y][x]; // 메모리 해제
				map[y][x] = new Ground(Vector2(x, y)); // Ground로 변경
			}
		}
	}
}

void GameLevel::FindPath()
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
			if (dynamic_cast<Wall*>(map[y][x]) 
				|| dynamic_cast<Clear*>(map[y][x]))
			{
				grid[y][x] = 1; // 장애물
			}
		}
	}

	// A* 알고리즘 실행
	AStar* astar = new AStar();
	Node startNode(start->Position());
	Node goalNode(goal->Position());

	// 목표 지점이 바뀌면 기존 경로를 완전히 새로 탐색
	if (goal->IsResetGoal() && !path.empty())
	{
		goal->ResetGoalFlag();

		// path가 비어 있거나 pathIndex가 0보다 작다면 
		// trimmedPath를 만들지 않도록 리턴
		if (path.empty() || pathIndex <= 0) return;

		// 기준 경로의 현재 진행 중인 부분을 유지
		std::vector<Node*> trimmedPath(path.begin(), path.begin() + pathIndex);

		// 기존 경로를 저장 (path의 마지막 점을 새 출발점으로 설정)
		Node lastPathNode(trimmedPath.back()->Position());
		
		// 새로운 출발점을 기존 경로의 마지막 노드로 설정
		std::vector<Node*> newPath = astar->FindPath(&lastPathNode, &goalNode, grid);

		//기존 pathIndex까지 유지한 후, 새로운 경로를 추가
		path = trimmedPath;
		path.insert(path.end(), newPath.begin(), newPath.end());

		// 기존 경로에서 이어서 탐색
		pathIndex = (int)trimmedPath.size();
	}
	// 시작점 갱신
	else if(start->IsResetStart())
	{
		start->ResetStartFlag();

		// 기존 경로 삭제
		ClearPreviousPath();

		path = astar->FindPath(&startNode, &goalNode, grid);
		pathIndex = 0; // 초기 경로 인덱스 설정
	}
}
