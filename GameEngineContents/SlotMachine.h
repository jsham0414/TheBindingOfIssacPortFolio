#pragma once

class SlotMachine : public GameEngineActor {
public:
	SlotMachine();
	~SlotMachine();

	// delete Function
	SlotMachine(const SlotMachine& _Other) = delete;
	SlotMachine(SlotMachine&& _Other) noexcept = delete;
	SlotMachine& operator=(const SlotMachine& _Other) = delete;
	SlotMachine& operator=(SlotMachine&& _Other) noexcept = delete;

protected:

	virtual void Start();
	virtual void Update(float _DeltaTime);
	virtual void End() {}

	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	void TriggerStart(const StateInfo& _Info);
	void TriggerUpdate(float _DeltaTime, const StateInfo& _Info);

	void SpinStart(const StateInfo& _Info);
	void SpinUpdate(float _DeltaTime, const StateInfo& _Info);

	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);

	void CreateFrameAnimation();

public:
	GameEngineStateManager StateManager;

protected:
	GameEngineTextureRenderer* Card[3];
	float4 TableNumber;
	int CardNumber[3] = { 0 };
	int Lock = 0;
	bool Spin = false;
};

