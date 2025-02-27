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
#include "Math/Vector2.h"

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
	// 좌클릭으로 에너미 생성하기
	if (Engine::Get().GetKeyDown(VK_LBUTTON))
	{
		Vector2 newEnemyPos = Engine::Get().MousePosition();
		AddEnemy(newEnemyPos);
	}

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
		FindPathEnemies();
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

		for (auto& pair : enemyPaths)
		{
			Start* enemy = pair.first;
			std::vector<Node*>& enemyPath = pair.second;

			if (pathsIndex < (int)enemyPath.size())
			{
				if (!enemyPath.empty())
				{
					Vector2 prevPosition = enemy->Position();
					Node* currentNode = enemyPath[pathsIndex];

					if (!currentNode) return;
					Vector2 pos = currentNode->Position();

					map[prevPosition.y][prevPosition.x] = new Ground(prevPosition);
					enemy->SetPosition(pos);
					map[pos.y][pos.x] = enemy;

					if (enemyPath.empty()) return;
					enemyPath.erase(enemyPath.begin()/* + pathsIndex*/); // 이동한 경로 삭제

					++pathsIndex;
				}
			}

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

	for (auto& pair : enemyPaths)
	{
		pair.first->Draw();
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
	int mapY = static_cast<int>(map.size());
	int mapX = static_cast<int>(map[0].size());
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

		// 기존 경로를 유지하면서 새로운 탐색을 진행하도록 설정
		pathIndex = static_cast<int>(trimmedPath.size());
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

void GameLevel::AddEnemy(Vector2 newEnemyPos)
{
	// 그리드 생성
	int mapY = static_cast<int>(map.size());
	int mapX = static_cast<int>(map[0].size());
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

	Actor* actor = map[newEnemyPos.y][newEnemyPos.x];

	// 이동할 위치가 맵을 벗어났거나 Wall, Clear 라면 리턴
	if (newEnemyPos.y > map.size()
		|| newEnemyPos.x > map[0].size()
		|| dynamic_cast<Wall*>(actor)
		|| dynamic_cast<Clear*>(actor)) return;

	// 새로운 적 추가
	Start* newEnemy = new Start(newEnemyPos, this);
	map[newEnemyPos.y][newEnemyPos.x] = newEnemy; // 맵에 반영

	// A* 알고리즘 실행
	AStar* astar = new AStar();
	Node startNode(newEnemy->Position());
	Node goalNode(goal->Position());

	std::vector<Node*> newPath = astar->FindPath(&startNode, &goalNode, grid);

	// 적과 해당 경로를 `map`에 저장
	enemyPaths[newEnemy] = newPath;
	
	// @Todo: 경로 갱신 시 인덱스값도 배열에 담아줘야할 듯
	pathsIndex = 0;

	/*if (goal->IsResetGoal() && !enemyPaths.empty())
	{
		// (enemyPaths) 모든 적들의 경로도 기존 경로를 유지하면서 새로운 목표로 이동
		for (auto& pair : enemyPaths)
		{
			Start* enemy = pair.first;
			std::vector<Node*>& enemyPath = pair.second;

			if (enemyPath.empty() || pathsIndex <= 0) return;

			// 적의 기존 경로 유지
			std::vector<Node*> trimmedEnemyPath(enemyPath.begin(), enemyPath.begin() + pathsIndex);

			// 적의 기존 경로의 마지막 점을 새로운 출발점으로 설정
			Node lastEnemyPathNode(trimmedEnemyPath.back()->Position());

			// 적의 새로운 경로 찾기
			std::vector<Node*> newEnemyPath = astar->FindPath(&lastEnemyPathNode, &goalNode, grid);

			// 기존 경로 + 새로운 경로를 합침
			enemyPath = trimmedEnemyPath;
			enemyPath.insert(enemyPath.end(), newEnemyPath.begin(), newEnemyPath.end());
		}
	}*/
}

void GameLevel::FindPathEnemies()
{
	// 그리드 생성
	int mapY = static_cast<int>(map.size());
	int mapX = static_cast<int>(map[0].size());
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
	Node goalNode(goal->Position());

	//if (goal->IsResetGoal() /*&& !enemyPaths.empty()*/)
	//{
		// (enemyPaths) 모든 적들의 경로도 기존 경로를 유지하면서 새로운 목표로 이동
		for (auto& pair : enemyPaths)
		{
			Start* enemy = pair.first;
			std::vector<Node*>& enemyPath = pair.second;

			if (enemyPath.empty() || pathsIndex <= 0) return;

			// 적의 기존 경로 유지
			std::vector<Node*> trimmedEnemyPath(enemyPath.begin(), enemyPath.begin() + pathsIndex);

			// 적의 기존 경로의 마지막 점을 새로운 출발점으로 설정
			Node lastEnemyPathNode(trimmedEnemyPath.back()->Position());

			// 적의 새로운 경로 찾기
			std::vector<Node*> newEnemyPath = astar->FindPath(&lastEnemyPathNode, &goalNode, grid);

			// 기존 경로 + 새로운 경로를 합침
			enemyPath = trimmedEnemyPath;
			enemyPath.insert(enemyPath.end(), newEnemyPath.begin(), newEnemyPath.end());

			// 기존 경로를 유지하면서 새로운 탐색을 진행하도록 설정
			pathsIndex = static_cast<int>(trimmedEnemyPath.size());
		}
	//}
}
