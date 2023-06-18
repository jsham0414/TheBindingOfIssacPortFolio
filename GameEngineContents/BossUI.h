#pragma once


class BossUI : public GameEngineActor {
public:
	BossUI();
	~BossUI();

	// delete Function
	BossUI(const BossUI& _Other) = delete;
	BossUI(BossUI&& _Other) noexcept = delete;
	BossUI& operator=(const BossUI& _Other) = delete;
	BossUI& operator=(BossUI&& _Other) noexcept = delete;

	void SetHp(float _X);
protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}
};

