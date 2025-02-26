#pragma once

#include "Actor/DrawableActor.h"
#include "Level/GameLevel.h"

class Goal : public DrawableActor
{
	RTTI_DECLARATIONS(Goal, DrawableActor)

public:
	Goal(const Vector2& position, GameLevel* level);

	virtual void Update(float deltaTime) override;

	void CanMove(Vector2 newPosition);

	bool IsResetGoal() { return bIsResetGoal; }
	void ResetGoalFlag() { bIsResetGoal = false; }

	bool IsClear() { return bIsClear; }
	bool IsOver() { return bIsOver; }

private:
	GameLevel* refLevel = nullptr;
	bool bIsResetGoal = false;
	bool bIsClear = false;
	bool bIsOver = false;
};