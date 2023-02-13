#pragma once
#include <GameEngineCore/GameEngineActor.h>

// Ό³Έν :
class LoginUI : public GameEngineActor
{
public:
	// constrcuter destructer
	LoginUI();
	~LoginUI();

	// delete Function
	LoginUI(const LoginUI& _Other) = delete;
	LoginUI(LoginUI&& _Other) noexcept = delete;
	LoginUI& operator=(const LoginUI& _Other) = delete;
	LoginUI& operator=(LoginUI&& _Other) noexcept = delete;

protected:

	void Start() override;
	void Update(float _DeltaTime) override;
	void End() override;
	void CreateFrameAnimation();
	void KeyBinding();

	void LogoUpdate(float _DeltaTime, const StateInfo& _Info);
	void SelectUpdate(float _DeltaTime, const StateInfo& _Info);

	void LogoStart(const StateInfo& _Info);
	void SelectStart(const StateInfo& _Info);

	void SetArrowPosition();

private:
	GameEngineStateManager StateManager;
	GameEngineTextureRenderer* DrawRenderer, * LogoRenderer;
	std::vector<GameEngineTextureRenderer*> SelectMenuButtons;
	GameEngineTextureRenderer* SelectArrow;

	bool IsLoading;
	int ArrowIndex;
	int MenuIndex;
	float TimeAngle;
};

