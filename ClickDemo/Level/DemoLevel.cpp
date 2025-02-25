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
	: lastUpdateTime(0.0f), pathIndex(0), drawTimer(0.25f)
{
	// �� ���� �ҷ�����
	ParseMap("../Assets/Map2.txt");

}

void DemoLevel::ParseMap(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cerr << "������ �о�� �� �����ϴ�.\n";
	}

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

	// 0.5�ʸ��� ����
	drawTimer.Update(deltaTime);

	if (drawTimer.IsTimeOut())
	{
		drawTimer.Reset();

		if (pathIndex < (int)path.size()-1)
		{
			// ��� �Ѱ��� ��������
			Node* currentNode = path[pathIndex];
			Vector2 pos = currentNode->Position();

			// ���� Ground �ڸ��� Path ��ü ����
			map[pos.y][pos.x] = new Path(pos);

			++pathIndex;
		}

		Draw();
	}
	/*// 1�ʸ��� ����� ���� ��带 ������
	lastUpdateTime += deltaTime;
	if (lastUpdateTime >= 1.0f && pathIndex < path.size())
	{
		lastUpdateTime = 0.0f;
		
		// ��ο��� �� ���� ��带 �����ͼ� ǥ��
		Node* currentNode = path[pathIndex];
		Vector2 pos = currentNode->Position();

		// ���� Ÿ���� �����ϰ� Path ��ü ����
		delete map[pos.y][pos.x];
		map[pos.y][pos.x] = new Path(pos);

		// ���� ��η� �̵�
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

void DemoLevel::ClearPreviousPath()
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

void DemoLevel::FindPath()
{
	if (!start || !goal) return;

	// ���� ��� ����
	//ClearPreviousPath();

	// ���� ��θ� ���� (path�� ������ ���� �� ��������� ����)
	Node* lastPathNode = nullptr;
	if (!path.empty())
	{
		lastPathNode = path.back(); // ���������� Draw�� ���
	}

	// �׸��� ����
	int mapY = (int)map.size();
	int mapX = (int)map[0].size();
	std::vector<std::vector<int>> grid(mapY, std::vector<int>(mapX, 0));

	for (int y = 0; y < mapY; ++y)
	{
		for (int x = 0; x < mapX; ++x)
		{
			if (dynamic_cast<Wall*>(map[y][x]))
			{
				grid[y][x] = 1; // ��ֹ�
			}
		}
	}

	// A* �˰��� ����
	AStar* astar = new AStar();
	
	// ���ο� ������� ���� ����� ������ ���� ����
	Node startNode = lastPathNode ? *lastPathNode : Node(start->Position());
	//Node startNode(start->Position());
	Node goalNode(goal->Position());

	path = astar->FindPath(&startNode, &goalNode, grid);

	//std::vector<Node*> newPath = astar->FindPath(&startNode, &goalNode, grid);

	// ���� path�� ���ο� ��θ� �߰�
	//path.insert(path.end(), newPath.begin(), newPath.end());

	// �ʱ� ��� �ε��� ����
	pathIndex = 0;
}