#pragma once
#include <GameEngineContents/Enums.h>

class GameEngineTransform;

class Door : public GameEngineActor {
public:
	Door();
	~Door();

	// delete Function
	Door(const Door& _Other) = delete;
	Door(Door&& _Other) noexcept = delete;
	Door& operator=(const Door& _Other) = delete;
	Door& operator=(Door&& _Other) noexcept = delete;

	GameEngineTransform* GetSpawnTransform() const {
		return Spawn;
	}
	void Initialize();

	GameEngineStateManager& GetState() {
		return StateManager;
	}

	void SetType(GameRoomType _Type);
protected:
	void Start() override;
	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}

	void BattleStart(const StateInfo& _Info);
	void IdleStart(const StateInfo& _Info);

	void BattleUpdate(float _DeltaTime, const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	GameEngineStateManager StateManager;

	GameEngineTransform* Spawn;

	GameRoomType Type;
};

