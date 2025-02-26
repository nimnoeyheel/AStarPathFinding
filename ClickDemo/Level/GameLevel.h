#pragma once

#include "Level/Level.h"
#include <vector>
#include "../Engine/Engine/Timer.h"

class Node;
class GameLevel : public Level
{
	// RTTI Á¤ÀÇ.
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();

	virtual void Update(float deltaTime) override;
	virtual void Draw() override;

	void ParseMap(const std::string& fileName);
    std::vector<std::vector<Actor*>> GetMap() { return map; }

	void ClearPreviousPath();
	void FindPath();
	void SetRestartingPathFindingFlag(bool bIsResetGoal) { bShouldRestartPathFinding = bIsResetGoal; }

private:
	std::vector<std::vector<Actor*>> map;
	std::vector<Node*> path;
	class Start* start = nullptr;
	class Goal* goal = nullptr;
	class Clear* clear = nullptr;

	Timer drawTimer;
	float lastUpdateTime;
	int pathIndex;
	bool bShouldRestartPathFinding = false;
};