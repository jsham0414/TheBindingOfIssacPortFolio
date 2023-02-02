#pragma once
#include <GameEngineCore/CoreMinimal.h>
#include <queue>
#include <GameEngineContents/PlayMap.h>
#include "../GameEngineCore/jsoncpp/json/json.h"
#include "../GameEngineCore/GameEngineLevel.h"
#include "Door.h"
#include "Wall.h"

// Ό³Έν :
class GameEngineCollision;
class GameEngineTextureRenderer;
class MapManager : public GameEngineActor {
public:
	MapManager();
	~MapManager();

	// delete Function
	MapManager(const MapManager& _Other) = delete;
	MapManager(MapManager&& _Other) noexcept = delete;
	MapManager& operator=(const MapManager& _Other) = delete;
	MapManager& operator=(MapManager&& _Other) noexcept = delete;

	void RandomMapGenerate(GameEngineLevel* _Level);

	int GetCurRoomOrder() {
		return CurRoomIndex + 10000;
	}

	static PlayMap* Load(const std::string& _Path) {

	}

	void MoveRoom(int _Direction = 0);
	void MoveRoomPos(float4 _DestPos, int _Direction = 0);
	void TurnOffLastRoom();

protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	PlayMap* GetCurrentMap();
	PlayMap* GetMap(int _Index);
	int GetCurMapEnemyCount();
	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void LoadMapFormData();
	bool Visit(int i);
	int NCount(int i);

	GameEngineTextureRenderer* Renderer;

private:
	int Width;
	int Height;
	int MaxRoomCount;
	int MinRoomCount;
	int RoomCount;
	int* RoomPlan;
	int StartPos;
	int SpecialRoomCount;
	int CurRoomIndex;
	int LastRoomIndex;
	const static int Move[4];

	std::vector<int> EndRoom;
	std::queue<int> RoomQueue;
	std::map<int, PlayMap*> PlayMaps;
	std::map<int, std::vector<std::string>> MapFormData;

	std::thread MapMoveThread;
};

