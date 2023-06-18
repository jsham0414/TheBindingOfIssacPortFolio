#pragma once
#include <GameEngineCore/GameEngineActor.h>
#include "Enums.h"

// Ό³Έν :
class Monster : public GameEngineActor {
public:
	// constrcuter destructer
	Monster();
	~Monster();

	void DamageAnimation();

	// delete Function
	Monster(const Monster& _Other) = delete;
	Monster(Monster&& _Other) noexcept = delete;
	Monster& operator=(const Monster& _Other) = delete;
	Monster& operator=(Monster&& _Other) noexcept = delete;

	virtual void Damage(float _Value = 1.f);
protected:
	virtual void Start() {}
	virtual void Update(float _DeltaTime) {}
	virtual void End() {}

	GameBloodType BloodType;

	int Hp;
	bool Dead;
	float MoveSpeed;
private:
	GameEngineStateManager StateManager;
};

