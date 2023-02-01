#pragma once

class HpInstance : public GameEngineActor {
public:
	HpInstance();
	~HpInstance();

	// delete Function
	HpInstance(const HpInstance& _Other) = delete;
	HpInstance(HpInstance&& _Other) noexcept = delete;
	HpInstance& operator=(const HpInstance& _Other) = delete;
	HpInstance& operator=(HpInstance&& _Other) noexcept = delete;

	void SetHp(int _Hp);

	int GetIndex();

	void SetIndex(int _Index);

protected:
	void Start() override;
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}

	int Index;
	int Hp;

};

