#pragma once

#include "Actor/DrawableActor.h"

class Start : public DrawableActor
{
	RTTI_DECLARATIONS(Start, DrawableActor)

public:
	Start(const Vector2& position);

	virtual void Update(float deltaTime) override;
};