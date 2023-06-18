#pragma once
class PopUpMessage : public GameEngineActor {
public:
	PopUpMessage();
	~PopUpMessage();

	// delete Function
	PopUpMessage(const PopUpMessage& _Other) = delete;
	PopUpMessage(PopUpMessage&& _Other) noexcept = delete;
	PopUpMessage& operator=(const PopUpMessage& _Other) = delete;
	PopUpMessage& operator=(PopUpMessage&& _Other) noexcept = delete;

	void SetTitle(const std::string& _Text, const std::string& _Font = "upheavtt.ttf");
	void SetText(const std::string& _Text, const std::string& _Font = "upheavtt.ttf");

	void Play();
protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	void MovingStart(const StateInfo& _Info);

	void MovingUpdate(float _DeltaTime, const StateInfo& _Info);

	void WaitingStart(const StateInfo& _Info);

	void WaitingUpdate(float _DeltaTime, const StateInfo& _Info);

	void EndStart(const StateInfo& _Info);

	void EndUpdate(float _DeltaTime, const StateInfo& _Info);

	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);
protected:
	GameEngineStateManager StateManager;

	std::unordered_map<std::string, GameEngineFontRenderer*> FontRenderer;

	ULONGLONG StartTime;

	float StartX, EndX;
};

