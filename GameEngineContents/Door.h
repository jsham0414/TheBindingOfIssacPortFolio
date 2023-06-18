#pragma once
#include <GameEngineContents/Enums.h>

class GameEngineTransform;

class Door : public GameEngineActor {
public:
	Door();
	virtual ~Door();

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
	void CreateFrameAnimation();
	void SetRendererState(bool _State);

	std::map<std::string, GameEngineTextureRenderer*>* GetDoorRenderer() {
		return &DoorRenderer;
	}

	bool IsOpen() {
		return Open;
	}

protected:
	void Start() override;
	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void Update(float _DeltaTime);
	void End() {}

	void CloseStart(const StateInfo& _Info);
	void OpenStart(const StateInfo& _Info);
	void FakeOpenStart(const StateInfo& _Info);

	void CloseUpdate(float _DeltaTime, const StateInfo& _Info);
	void OpenUpdate(float _DeltaTime, const StateInfo& _Info);
	void FakeOpenUpdate(float _DeltaTime, const StateInfo& _Info);

	

	void WallStart(const StateInfo& _Info);

	GameEngineStateManager StateManager;

	GameEngineTransform* Spawn;
	std::map<std::string, GameEngineTextureRenderer*> DoorRenderer;

	DWORD StartTime;
	float OpenTime;
	float4 DoorSize;

	bool Open;

	GameRoomType Type;
};

