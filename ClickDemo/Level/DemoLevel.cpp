#include "DemoLevel.h"
#include "Actor/Goal.h"
#include "Actor/Start.h"
#include "Actor/Ground.h"
#include "Actor/Wall.h"
#include <string>
#include <fstream>
#include <Engine/Engine.h>

bool DemoLevel::ParseMap(const std::string& fileName)
{
	// 파일 열기
	//FILE* file = nullptr;
	//fopen_s(&file, path, "r");
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cerr << "파일을 읽어올 수 없습니다.\n";
		return false;
	}

	//char buffer[2048] = {};

	map.clear();

	int y = 0;
	std::string line;

	// 한 줄씩 읽어서 처리
	//while (fgets(buffer, sizeof(buffer), file))
	while (std::getline(file, line));
	{
		std::vector<Actor*> mapData;
		//char* context = nullptr;

		// 첫 줄 처리
		//char* splitString = strtok_s(buffer, ",", &context);
		for (int x = 0; x < line.size(); ++x)
		{
			char mapChar = line[x];
			if (mapChar == '0') // Ground
			{
				mapData.emplace_back(new Ground(Vector2(x, y)));
			}
			else if (mapChar == '1') // Wall
			{
				mapData.emplace_back(new Wall(Vector2(x, y)));
			}
			else if (mapChar == 's') // Start
			{
				mapData.emplace_back(new Start(Vector2(x, y)));
			}
			else if (mapChar == 'e') // Goal
			{
				mapData.emplace_back(new Goal(Vector2(x, y)));
			}
			else
			{
				mapData.emplace_back(nullptr);
			}
		}
		map.emplace_back(mapData);
		++y;

		//// 나머지 줄 처리
		//while (context)
		//{
		//	splitString = strtok_s(nullptr, ",", &context);
		//	if (!splitString) break;
		//	line.emplace_back(splitString[0]);
		//}

		//// 처리된 라인 데이터를 맵에 추가
		//map.emplace_back(line);
	}

	file.close();
	return true;
}

DemoLevel::DemoLevel()
{
	const std::string fileName = "../Assets/Map1.txt";

	//if (ParseMap("../Assets/Map1.txt"))
	//{
	//}

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
			if (mapChar == '0') // Ground
			{
				mapData.emplace_back(new Ground(Vector2(x, y)));
			}
			else if (mapChar == '1') // Wall
			{
				mapData.emplace_back(new Wall(Vector2(x, y)));
			}
			else if (mapChar == 's') // Start
			{
				mapData.emplace_back(new Start(Vector2(x, y)));
			}
			else if (mapChar == 'e') // Goal
			{
				mapData.emplace_back(new Goal(Vector2(x, y)));
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


