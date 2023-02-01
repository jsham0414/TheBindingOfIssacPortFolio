#pragma once
#include <GameEngineCore/GameEngineLevel.h>

// ���� :
class MapObjectBatcher;
class MapEditorWindow;
class MapEditorLevel : public GameEngineLevel
{
public:
	// constrcuter destructer
	MapEditorLevel();
	~MapEditorLevel();

	// delete Function
	MapEditorLevel(const MapEditorLevel& _Other) = delete;
	MapEditorLevel(MapEditorLevel&& _Other) noexcept = delete;
	MapEditorLevel& operator=(const MapEditorLevel& _Other) = delete;
	MapEditorLevel& operator=(MapEditorLevel&& _Other) noexcept = delete;

	TileMapActor* TileMap;

protected:
	void Start() override;
	void LevelStartEvent() override;
	void LoadAllObject();
	void Update(float _DeltaTime) override;
	void End() override;

	void LoadTexture();
private:
	vector<GameEngineActor> Actors;
	MapObjectBatcher* ObjectBatcher;
	MapEditorWindow* TileMapEditorGUI;

	//void OnEvent() override;

};

