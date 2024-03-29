#pragma once

class Bomb : public GameEngineActor {
public:
	// constrcuter destructer
	Bomb();
	~Bomb();

	// delete Function
	Bomb(const Bomb& _Other) = delete;
	Bomb(Bomb&& _Other) noexcept = delete;
	Bomb& operator=(const Bomb& _Other) = delete;
	Bomb& operator=(Bomb&& _Other) noexcept = delete;

	bool ObjectCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	bool DamageCollision(GameEngineCollision* _This, GameEngineCollision* _Other);

protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	void CreateFrameAnimation();
	void BombDelay(DWORD _Time);
	void ColorChange(DWORD _Time);

	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	void DeadStart(const StateInfo& _Info);
	void DeadUpdate(float _DeltaTime, const StateInfo& _Info);

	std::thread DelayThread;
	float4 LastVelocity;
	int Index;

	float4 Push;
	float4 Accel;

	DWORD DelayTime;
	DWORD LastTime;
	DWORD StartTime;
	DWORD ExplosionTime;

	bool CollisionTrigger;

	GameEngineStateManager StateManager;
};

