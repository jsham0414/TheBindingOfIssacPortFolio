#pragma once
#include "Monster.h"

class Horf : public Monster {
public:
	Horf();
	~Horf();

	// delete Function
	Horf(const Horf& _Other) = delete;
	Horf(Horf&& _Other) noexcept = delete;
	Horf& operator=(const Horf& _Other) = delete;
	Horf& operator=(Horf&& _Other) noexcept = delete;

	virtual void Damage();

protected:
	float Power;
	float Period;
	DWORD AttackDelay;
	DWORD LastDelay;
	float Size;

	int Hp;

	bool ShootOnce;

	virtual void Start();
	virtual void Update(float _DeltaTime);
	virtual void End() {}

	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void CreateFrameAnimation();

	GameEngineStateManager StateManager;
private:
	void IdleStateStart(const StateInfo& _Info);
	void IdleStateUpdate(float _DeltaTime, const StateInfo& _Info);

	void AttackStateStart(const StateInfo& _Info);
	void AttackStateUpdate(float _DeltaTime, const StateInfo& _Info);

	void ShootBullet();
};

