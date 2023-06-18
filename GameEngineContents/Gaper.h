#pragma once
#include "Monster.h"
#include <GameEngineCore/GameEngineAStar.h>

class GameEngineAStar;
class Gaper : public Monster {
public:
	Gaper();
	~Gaper();

	// delete Function
	Gaper(const Gaper& _Other) = delete;
	Gaper(Gaper&& _Other) noexcept = delete;
	Gaper& operator=(const Gaper& _Other) = delete;
	Gaper& operator=(Gaper&& _Other) noexcept = delete;

protected:
	virtual void Start();
	virtual void Update(float _DeltaTime);
	virtual void End() {}

	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void CreateFrameAnimation();

	std::map<std::string, GameEngineTextureRenderer*> GaperRenderer;

	GameEngineAStar* AStar;
	GameEngineStateManager StateManager;

private:
	float LastTime;
	POINT NextPos;
	int Direction;
};

