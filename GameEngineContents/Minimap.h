#pragma once
#include "MapManager.h"

class MinimapIcon;
class Minimap : public GameEngineActor {
public:
	Minimap();
	~Minimap();

	// delete Function
	Minimap(const Minimap& _Other) = delete;
	Minimap(Minimap&& _Other) noexcept = delete;
	Minimap& operator=(const Minimap& _Other) = delete;
	Minimap& operator=(Minimap&& _Other) noexcept = delete;

	void MoveMap(int _Index);
protected:
	void Start() override;
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}


private:
	int CurMapIndex;
	std::map<int, MinimapIcon*> IconRenderer;
};
