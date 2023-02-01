#pragma once
#include "UINumber.h"

class StuffGuider : public GameEngineActor {
public:
	StuffGuider();
	~StuffGuider();

	// delete Function
	StuffGuider(const StuffGuider& _Other) = delete;
	StuffGuider(StuffGuider&& _Other) noexcept = delete;
	StuffGuider& operator=(const StuffGuider& _Other) = delete;
	StuffGuider& operator=(StuffGuider&& _Other) noexcept = delete;

	void ChangeValue(int _Value);

protected:
	void Start() override;
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}

	UINumber* NumberInstance;

	int Value;
};

