#pragma once
#include <GameEngineCore/GameEngineActor.h>

// Ό³Έν :
class UIMaster : public GameEngineActor
{
public:
	// constrcuter destructer
	UIMaster();
	~UIMaster();

	// delete Function
	UIMaster(const UIMaster& _Other) = delete;
	UIMaster(UIMaster&& _Other) noexcept = delete;
	UIMaster& operator=(const UIMaster& _Other) = delete;
	UIMaster& operator=(UIMaster&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

private:

};

