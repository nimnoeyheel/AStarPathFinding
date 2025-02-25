#pragma once

#include "Actor/DrawableActor.h"

class Path : public DrawableActor
{
	RTTI_DECLARATIONS(Path, DrawableActor)

public:
	Path(const Vector2& position);
};