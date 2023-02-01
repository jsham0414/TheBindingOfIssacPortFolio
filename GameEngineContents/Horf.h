#pragma once

class Horf : public GameEngineActor {
public:
	Horf();
	~Horf();

	// delete Function
	Horf(const Horf& _Other) = delete;
	Horf(Horf&& _Other) noexcept = delete;
	Horf& operator=(const Horf& _Other) = delete;
	Horf& operator=(Horf&& _Other) noexcept = delete;

protected:
	float Power;
	float Period;
	DWORD AttackDelay;
	DWORD LastDelay;
	float Size;

	int Hp;

	bool ShootOnce;

	void Start() override;
	void Update(float _DeltaTime);
	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void End() {}
	void CreateFrameAnimation();
	Horf* Clone() {

	};

	GameEngineStateManager StateManager;
private:
	void IdleStateStart(const StateInfo& _Info);
	void IdleStateUpdate(float _DeltaTime, const StateInfo& _Info);

	void AttackStateStart(const StateInfo& _Info);
	void AttackStateUpdate(float _DeltaTime, const StateInfo& _Info);

	void ShootBullet();
};

