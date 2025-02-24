#pragma once

#include "Level/Level.h"
#include <vector>

class DemoLevel : public Level
{
	// RTTI Á¤ÀÇ.
	RTTI_DECLARATIONS(DemoLevel, Level)

public:
	DemoLevel();

	virtual void Update(float deltaTime) override;
	virtual void Draw() override;

	bool ParseMap(const std::string& fileName);

private:
	std::vector<std::vector<Actor*>> map;

};