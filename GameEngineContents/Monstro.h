#pragma once
#include <GameEngineContents/BossUI.h>
class Monstro : public Monster {
public:
	Monstro();
	~Monstro();

	// delete Function
	Monstro(const Monstro& _Other) = delete;
	Monstro(Monstro&& _Other) noexcept = delete;
	Monstro& operator=(const Monstro& _Other) = delete;
	Monstro& operator=(Monstro&& _Other) noexcept = delete;

	void Damage(float _Value = 1.f) override;
protected:
	float Speed;
	float Period;
	DWORD AttackDelay;
	float BigJumpTime; // 날아가는데 걸릴 시간 (초)

	bool Jumping;
	bool Falling;

	float JumpWidth; // 얼마나 높이 올라갈 건지
	float Size;
	float Direction;

	float4 TextureSize, ShadowSize;

	int Hp;
	int MaxHp;

	bool ShootOnce;

	virtual void Start();
	virtual void Update(float _DeltaTime);
	virtual void End() {}

	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void CreateFrameAnimation();

	GameEngineStateManager StateManager;

	std::map<std::string, GameEngineTextureRenderer*> MonstroRenderer;

	float4 StartPos, LastPlayerPosition;

	BossUI* BossUIInstance;

	virtual void OnEvent() override;
	virtual void OffEvent() override;
private:
	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	void BigJumpStart(const StateInfo& _Info);
	void BigJumpUpdate(float _DeltaTime, const StateInfo& _Info);

	void BigJumpingStart(const StateInfo& _Info);
	void BigJumpingUpdate(float _DeltaTime, const StateInfo& _Info);

	void BigJumpEndStart(const StateInfo& _Info);
	void BigJumpEndUpdate(float _DeltaTime, const StateInfo& _Info);

	void JumpStart(const StateInfo& _Info);
	void JumpUpdate(float _DeltaTime, const StateInfo& _Info);

	void SpitStart(const StateInfo& _Info);
	void SpitUpdate(float _DeltaTime, const StateInfo& _Info);

	void SpitingStart(const StateInfo& _Info);
	void SpitingUpdate(float _DeltaTime, const StateInfo& _Info);

	void DeadStart(const StateInfo& _Info);
	void DeadUpdate(float _DeltaTime, const StateInfo& _Info);

	void RandomPatern();
	void TurnToPlayer();
	void BigJumpAttack();
};

