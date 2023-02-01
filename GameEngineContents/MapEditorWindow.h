#pragma once
#include <GameEngineCore/GameEngineGUI.h>
#include "MapObjectBatcher.h"
#include "GameEngineCore/jsoncpp/json/json.h"

// Ό³Έν :
class TileMapActor;
class MapEditorWindow : public GameEngineGUIWindow
{
public:
	// constrcuter destructer
	MapEditorWindow();
	~MapEditorWindow();

	// delete Function
	MapEditorWindow(const MapEditorWindow& _Other) = delete;
	MapEditorWindow(MapEditorWindow&& _Other) noexcept = delete;
	MapEditorWindow& operator=(const MapEditorWindow& _Other) = delete;
	MapEditorWindow& operator=(MapEditorWindow&& _Other) noexcept = delete;

	TileMapActor* TileMap;
	std::string SelectFolderTexture;

	int Scale[2];

	size_t SelectTile;
	vector<ObjectBatchInfo*> BatchInfos;
	int Selected;

	void SetObjectBatcher(MapObjectBatcher* _Batcher) { ObjectBatcher = _Batcher; }

protected:

private:
	MapObjectBatcher* ObjectBatcher;

	void Initialize(class GameEngineLevel* _Level) override;
	void OnGUI(GameEngineLevel* _Level, float _DeltaTime) override;
};

