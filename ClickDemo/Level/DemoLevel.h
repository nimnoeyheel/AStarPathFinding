#pragma once

#include "Level/Level.h"
#include <vector>
#include "../Engine/Engine/Timer.h"

class Node;
class DemoLevel : public Level
{
	// RTTI Á¤ÀÇ.
	RTTI_DECLARATIONS(DemoLevel, Level)

public:
	DemoLevel();

	virtual void Update(float deltaTime) override;
	virtual void Draw() override;

	void ParseMap(const std::string& fileName);
    std::vector<std::vector<Actor*>> GetMap() { return map; }

	void ClearPreviousPath();
	void FindPath();

private:
	std::vector<std::vector<Actor*>> map;
	class Start* start = nullptr;
	class Goal* goal = nullptr;

	std::vector<Node*> path;
	float lastUpdateTime;
	int pathIndex;
	Timer drawTimer;
};