#pragma once
class UINumber : public GameEngineActor {
public:
	UINumber();
	~UINumber();

	// delete Function
	UINumber(const UINumber& _Other) = delete;
	UINumber(UINumber&& _Other) noexcept = delete;
	UINumber& operator=(const UINumber& _Other) = delete;
	UINumber& operator=(UINumber&& _Other) noexcept = delete;

	void ChangeValue(int _Value);

protected:
	void Start() override;
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}

	int Value;

	LinkedList<GameEngineTextureRenderer*> RendererList;
};

