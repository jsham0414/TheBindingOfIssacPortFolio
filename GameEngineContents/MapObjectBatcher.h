#pragma once
#include <GameEngineCore/CoreMinimal.h>
#include "BatchDummy.h"
#include <unordered_map>

// Ό³Έν :
class GameEngineCollision;
class GameEngineTextureRenderer;
class MapEditorWindow;
class MapObjectBatcher : public GameEngineActor {
public:
	MapObjectBatcher();
	~MapObjectBatcher();

	// delete Function
	MapObjectBatcher(const MapObjectBatcher& _Other) = delete;
	MapObjectBatcher(MapObjectBatcher&& _Other) noexcept = delete;
	MapObjectBatcher& operator=(const MapObjectBatcher& _Other) = delete;
	MapObjectBatcher& operator=(MapObjectBatcher&& _Other) noexcept = delete;

	void BatchObject(int _Index, ObjectBatchInfo* _Info);
	void DeleteObject(int _Index);

	map<int, BatchDummy*>* GetBatchData() {
		return &BatchData;
	};
protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

private:
	MapEditorWindow* EditorWindow;
	GameEngineTextureRenderer* Renderer;
	map<int, BatchDummy*> BatchData;

	int Width;
	int Height;

	float4 StartPos;
	float4 TexScale;
};

