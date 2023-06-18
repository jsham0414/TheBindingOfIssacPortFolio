#pragma once
#include <GameEngineCore/CoreMinimal.h>
#include <queue>
#include <GameEngineContents/PlayMap.h>
#include "../GameEngineCore/jsoncpp/json/json.h"
#include "../GameEngineCore/GameEngineLevel.h"
#include "Door.h"
#include "Wall.h"
#include "Minimap.h"

// 설명 :
class Minimap;
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

	void CheckEnemies(bool _Forced = false);
	void ObjectDestory(GameEngineActor* _Actor);

	int GetRoomType(int _Index);
	PlayMap* GetMap(int _Index);
	PlayMap* GetCurrentMap();
	int GetObjectSpawnType(int _X, int _Y);
protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	int GetCurMapEnemyCount();
	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void LoadMapFormData();
	bool Visit(std::unordered_map<int, int>& _RoomPlan, int i);
	int NCount(std::unordered_map<int, int>& _RoomPlan, int i);

	GameEngineTextureRenderer* Renderer;

private:
	int Width;
	int Height;

	const int RoomWidth;
	const int RoomHeight;

	int MaxRoomCount;
	int MinRoomCount;
	int RoomCount;
	int StartPos;
	int SpecialRoomCount;
	int CurRoomIndex;
	int LastRoomIndex;

	float4 StartOffset;
	int TileSize;

	constexpr static int Move[4] = { 10, -1, 1, -10 };

	// static 오브젝트들 충돌 체크용
	int ObjectSpawnType[7][13];
	std::vector<int> EndRoom;
	std::queue<int> RoomQueue;
	std::map<int, PlayMap*> PlayMaps;
	std::map<int, std::vector<std::string>> MapFormData;
	Minimap* MinimapUI;

	std::thread MapMoveThread;
};

