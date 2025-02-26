#pragma once

#include "Actor/DrawableActor.h"
#include "Level/GameLevel.h"

class Clear : public DrawableActor
{
	RTTI_DECLARATIONS(Clear, DrawableActor)

public:
	Clear(const Vector2& position, GameLevel* level);

	virtual void Update(float deltaTime) override;

private:
	GameLevel* refLevel = nullptr;
};