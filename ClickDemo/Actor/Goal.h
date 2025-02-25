#pragma once

#include "Actor/DrawableActor.h"
#include "Level/DemoLevel.h"

class Goal : public DrawableActor
{
	RTTI_DECLARATIONS(Goal, DrawableActor)

public:
	Goal(const Vector2& position, DemoLevel* level);

	virtual void Update(float deltaTime) override;

private:
	DemoLevel* refLevel;
};