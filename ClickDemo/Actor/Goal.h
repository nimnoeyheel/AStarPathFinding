#pragma once

#include "Actor/DrawableActor.h"

class Goal : public DrawableActor
{
	RTTI_DECLARATIONS(Goal, DrawableActor)

public:
	Goal(const Vector2& position);

	virtual void Update(float deltaTime) override;
};