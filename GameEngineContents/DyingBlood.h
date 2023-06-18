#pragma once
#include "Enums.h"

class DyingBlood : public GameEngineActor {
public:
	DyingBlood();
	~DyingBlood();

	// delete Function
	DyingBlood(const DyingBlood& _Other) = delete;
	DyingBlood(DyingBlood&& _Other) noexcept = delete;
	DyingBlood& operator=(const DyingBlood& _Other) = delete;
	DyingBlood& operator=(DyingBlood&& _Other) noexcept = delete;

	void SetType(GameBloodType _Type);
protected:
	void Start() override;
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}

private:
	GameEngineTextureRenderer* Effect;

	float PrevTime;
};

