#pragma once
#include <GameEngineCore/GameEngineActor.h>

// Ό³Έν :
class GameMenu : public GameEngineActor {
public:
	// constrcuter destructer
	GameMenu();
	~GameMenu();

	// delete Function
	GameMenu(const GameMenu& _Other) = delete;
	GameMenu(GameMenu&& _Other) noexcept = delete;
	GameMenu& operator=(const GameMenu& _Other) = delete;
	GameMenu& operator=(GameMenu&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	void End() {};
	void CreateFrameAnimation();

	void LogoUpdate(float _DeltaTime, const StateInfo& _Info);
	void SelectUpdate(float _DeltaTime, const StateInfo& _Info);

	void LogoStart(const StateInfo& _Info);
	void SelectStart(const StateInfo& _Info);

private:
	GameEngineStateManager StateManager;
	GameEngineTextureRenderer* DrawRenderer, * LogoRenderer;

	int MenuIndex;
	float TimeAngle;
};

