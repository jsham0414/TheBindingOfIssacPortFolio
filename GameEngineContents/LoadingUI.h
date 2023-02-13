#pragma once

class LoadingUI : public GameEngineActor {
public:
	LoadingUI();
	~LoadingUI();

	// delete Function
	LoadingUI(const LoadingUI& _Other) = delete;
	LoadingUI(LoadingUI&& _Other) noexcept = delete;
	LoadingUI& operator=(const LoadingUI& _Other) = delete;
	LoadingUI& operator=(LoadingUI&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	void FadeInStart(const StateInfo& _Info);
	void FadeInUpdate(float _DeltaTime, const StateInfo& _Info);

	void LoadingStart(const StateInfo& _Info);
	void LoadingUpdate(float _DeltaTime, const StateInfo& _Info);

	void FadeOutStart(const StateInfo& _Info);
	void FadeOutUpdate(float _DeltaTime, const StateInfo& _Info);

	GameEngineStateManager StateManager;
	GameEngineTextureRenderer* LoadingImage;
	DWORD StartTime;
	float FadeTime; // 페이드 인이 완료되기까지 시간
};

