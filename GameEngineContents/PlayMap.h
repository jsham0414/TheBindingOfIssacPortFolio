#pragma once
#include <GameEngineCore/CoreMinimal.h>
#include <GameEngineContents/Door.h>
#include <GameEngineContents/Wall.h>
#include "GameEngineCore/GameEngineCollision.h"
#include <GameEngineContents/Enums.h>



// Ό³Έν :
class GameEngineTextureRenderer;
class PlayMap : public GameEngineActor {
public:
	PlayMap();
	~PlayMap();

	// delete Function
	PlayMap(const PlayMap& _Other) = delete;
	PlayMap(PlayMap&& _Other) noexcept = delete;
	PlayMap& operator=(const PlayMap& _Other) = delete;
	PlayMap& operator=(PlayMap&& _Other) noexcept = delete;

	std::vector<GameEngineActor*>& GetIncludeActors() {
		return IncludeActors;
	}

	int RoomType;

	void UpdateCollision(std::function<bool(GameEngineCollision*, GameEngineCollision*)> _Function);

	void MapOn(int* _DoorOpen);
	void MapOff();

	std::vector<Wall*>& GetWalls() {
		return Walls;
	}

	std::map<int, Door*>& GetDoors() {
		return Doors;
	}

	GameEngineStateManager& GetState() {
		return StateManager;
	}

	void Initialize();

protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	void BattleStart(const StateInfo& _Info);
	void IdleStart(const StateInfo& _Info);
	void OffStart(const StateInfo& _Info);

	void BattleUpdate(float _DeltaTime, const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);
	void OffUpdate(float _DeltaTime, const StateInfo& _Info);

	void TurnOnActors();
	void TurnOffActors();
private:
	int Width;
	int Height;
	int MaxRoomCount;
	int MinRoomCount;
	int RoomCount;
	int* RoomPlan;
	int StartPos;
	int SpecialRoomCount;

	GameEngineStateManager StateManager;

	std::vector<GameEngineActor*> IncludeActors;

	std::vector<int> EndRoom;
	std::queue<int> RoomQueue;

	std::map<int, Door*> Doors;
	std::vector<Wall*> Walls;
};

