#pragma once

#include "Level/Level.h"
#include "Math/Vector2.h"
#include "../Engine/Engine/Timer.h"
#include <vector>
#include <map>

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

	void AddEnemy(Vector2 newEnemyPos);
	void FindPathEnemies();

private:
	std::vector<std::vector<Actor*>> map;
	std::vector<Node*> path;
	class Start* start = nullptr;
	class Goal* goal = nullptr;
	class Clear* clear = nullptr;

	std::map<Start*, std::vector<Node*>> enemyPaths;

	int pathsIndex = 0;

	Timer drawTimer;
	float lastUpdateTime;
	int pathIndex;
	bool bShouldRestartPathFinding = false;
};