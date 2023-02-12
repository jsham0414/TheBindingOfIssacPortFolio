#pragma once
#include <GameEngineCore/GameEngineActor.h>

// Ό³Έν :
class Monster : public GameEngineActor
{
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

	virtual void Damage() {}
protected:
	virtual void Start() {}
	virtual void Update(float _DeltaTime) {}
	virtual void End() {}

	std::thread ColorChange;
private:
	GameEngineStateManager StateManager;
};

