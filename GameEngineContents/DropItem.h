#pragma once
class DropItem : public GameEngineActor {
public:
	// constrcuter destructer
	DropItem();
	~DropItem();

	// delete Function
	DropItem(const DropItem& _Other) = delete;
	DropItem(DropItem&& _Other) noexcept = delete;
	DropItem& operator=(const DropItem& _Other) = delete;
	DropItem& operator=(DropItem&& _Other) noexcept = delete;

	void SetType(int _Type);
protected:
	virtual void Start();
	virtual void Update(float _DeltaTime);
	virtual void End() {}

	void InitStart(const StateInfo& _Info);
	void InitUpdate(float _DeltaTime, const StateInfo& _Info);

	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	void DeadStart(const StateInfo& _Info);
	void DeadUpdate(float _DeltaTime, const StateInfo& _Info);

	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
private:
	int Type;

	GameEngineStateManager StateManager;
};
