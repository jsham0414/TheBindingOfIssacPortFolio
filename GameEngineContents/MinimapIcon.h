#pragma once

class MinimapIcon : public GameEngineActor {
public:
	MinimapIcon();
	~MinimapIcon();

	// delete Function
	MinimapIcon(const MinimapIcon& _Other) = delete;
	MinimapIcon(MinimapIcon&& _Other) noexcept = delete;
	MinimapIcon& operator=(const MinimapIcon& _Other) = delete;
	MinimapIcon& operator=(MinimapIcon&& _Other) noexcept = delete;

	void SetIndex(int _Index);
	GameEngineStateManager StateManager;

	int Index;
protected:
	void Start() override;
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}

	void VisitStart(const StateInfo& _Info);
	//void VisitUpdate(float _DeltaTime, const StateInfo& _Info);
	void ClearedStart(const StateInfo& _Info);
	void UnknownStart(const StateInfo& _Info);

private:

};