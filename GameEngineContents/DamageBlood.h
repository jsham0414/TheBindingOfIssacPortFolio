#pragma once
#include <GameEngineBase/GameEngineMemoryPool.h>

class DamageBlood : public GameEngineMemoryPool<DamageBlood, 100>, public GameEngineActor {
public:
	DamageBlood();
	~DamageBlood();

	DamageBlood(const DamageBlood& _Other) = delete;
	DamageBlood(DamageBlood&& _Other) noexcept = delete;
	DamageBlood& operator=(const DamageBlood& _Other) = delete;
	DamageBlood& operator=(DamageBlood&& _Other) noexcept = delete;

protected:
	virtual void Start();
	virtual void Initialize() override;
	virtual void Update(float _DeltaTime);
	virtual void End() {}

	float AccumulateTime;
	float DeathTime;

	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
};

