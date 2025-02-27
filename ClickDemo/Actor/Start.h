#pragma once

#include "Actor/DrawableActor.h"
#include "Level/GameLevel.h"

class Start : public DrawableActor
{
	RTTI_DECLARATIONS(Start, DrawableActor)

public:
	Start(const Vector2& position, GameLevel* level);

	virtual void Update(float deltaTime) override;

	void CanMove(Vector2 newPosition);

	bool IsResetStart() { return bIsResetStart; }
	void ResetStartFlag() { bIsResetStart = false; }

	bool IsAddEnemy() { return bIsAddEnemy; }
	void AddEnemyFlag() { bIsAddEnemy = false; }

private:
	GameLevel* refLevel = nullptr;
	bool bIsResetStart = true;
	bool bIsAddEnemy = false;
};