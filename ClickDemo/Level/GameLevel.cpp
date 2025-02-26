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
	// �� ���� �ҷ�����
	ParseMap("../Assets/Map2.txt");
}

void GameLevel::ParseMap(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open()) return;

	map.clear();

	int y = 0;
	std::string line;

	// �� �پ� �о ó��
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

	// ���� ����
	if (goal->IsClear())
	{
		Engine::Get().LoadLevel(new MenuLevel("Game Clear!!"));
	}
	else if (goal->IsOver())
	{
		Engine::Get().LoadLevel(new MenuLevel("Game Over!!"));
	}

	// Goal ��ġ�� ���ŵ� ��, pathIndex�� ������ �κп������� �ٽ� Ž�� ����
	if (bShouldRestartPathFinding)
	{
		FindPath();
		bShouldRestartPathFinding = false;
		return;
	}

	// Ÿ�̸� ����
	drawTimer.Update(deltaTime);
	if (drawTimer.IsTimeOut())
	{
		drawTimer.Reset();
		if (pathIndex < (int)path.size())
		{
			// ���� Start ��ġ ����
			Vector2 prevPosition = start->Position();

			// ��� �� ���� ��������
			Node* currentNode = path[pathIndex];
			Vector2 pos = currentNode->Position();

			// ���� Start ��ġ�� Ground�� ����
			map[prevPosition.y][prevPosition.x] = new Ground(prevPosition);

			// ���� ��ġ�� Start ����
			start->SetPosition(pos);
			map[pos.y][pos.x] = start;

			// ���� Ground �ڸ��� Path ��ü ����
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
	// ���� Path�� ��� Ground�� ����
	for (int y = 0; y < (int)map.size(); ++y)
	{
		for (int x = 0; x < (int)map[0].size(); ++x)
		{
			if (dynamic_cast<Path*>(map[y][x]))
			{
				//delete map[y][x]; // �޸� ����
				map[y][x] = new Ground(Vector2(x, y)); // Ground�� ����
			}
		}
	}
}

void GameLevel::FindPath()
{
	if (!start || !goal) return;

	// �׸��� ����
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
				grid[y][x] = 1; // ��ֹ�
			}
		}
	}

	// A* �˰��� ����
	AStar* astar = new AStar();
	Node startNode(start->Position());
	Node goalNode(goal->Position());

	// ��ǥ ������ �ٲ�� ���� ��θ� ������ ���� Ž��
	if (goal->IsResetGoal() && !path.empty())
	{
		goal->ResetGoalFlag();

		// path�� ��� �ְų� pathIndex�� 0���� �۴ٸ� 
		// trimmedPath�� ������ �ʵ��� ����
		if (path.empty() || pathIndex <= 0) return;

		// ���� ����� ���� ���� ���� �κ��� ����
		std::vector<Node*> trimmedPath(path.begin(), path.begin() + pathIndex);

		// ���� ��θ� ���� (path�� ������ ���� �� ��������� ����)
		Node lastPathNode(trimmedPath.back()->Position());
		
		// ���ο� ������� ���� ����� ������ ���� ����
		std::vector<Node*> newPath = astar->FindPath(&lastPathNode, &goalNode, grid);

		//���� pathIndex���� ������ ��, ���ο� ��θ� �߰�
		path = trimmedPath;
		path.insert(path.end(), newPath.begin(), newPath.end());

		// ���� ��ο��� �̾ Ž��
		pathIndex = (int)trimmedPath.size();
	}
	// ������ ����
	else if(start->IsResetStart())
	{
		start->ResetStartFlag();

		// ���� ��� ����
		ClearPreviousPath();

		path = astar->FindPath(&startNode, &goalNode, grid);
		pathIndex = 0; // �ʱ� ��� �ε��� ����
	}
}
