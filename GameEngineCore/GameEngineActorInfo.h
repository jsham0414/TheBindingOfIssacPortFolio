#pragma once
#include "GameEngineComponent.h"

struct GameEngineInfo {
	void SetMaxHp(float _MaxHp) {
		MaxHp = _MaxHp;
		Hp = MaxHp;
	}

	float MaxHp, Hp;
};

// �ΰ��� ���͵��� HP�� ������ ��� ������Ʈ
class GameEngineActorInfo : public GameEngineComponent {
public:
	// constrcuter destructer
	GameEngineActorInfo();
	virtual ~GameEngineActorInfo();

	// delete Function
	GameEngineActorInfo(const GameEngineActorInfo& _Other) = delete;
	GameEngineActorInfo(GameEngineActorInfo&& _Other) noexcept = delete;
	GameEngineActorInfo& operator=(const GameEngineActorInfo& _Other) = delete;
	GameEngineActorInfo& operator=(GameEngineActorInfo&& _Other) noexcept = delete;

	GameEngineInfo& GetInfo() {
		return Info;
	}

private:
	GameEngineInfo Info;
};

