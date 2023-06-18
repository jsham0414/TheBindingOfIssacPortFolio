#pragma once
#include <GameEngineCore/CoreMinimal.h>
#include "Enums.h"

struct PlayerInfo {
	PlayerInfo() : BaseAttackSpeed(0.35f), BaseDamage(1.f) {}
	int HP;
	int MaxHP;
	float BonusDamage;
	float BaseDamage;
	float AttackSpeed;
	float BaseAttackSpeed;

	float GetAttackSpeed() {
		return BaseAttackSpeed - BaseAttackSpeed * (AttackSpeed / 100.f);
	}

	float GetDamage() {
		return BaseDamage + BaseDamage * (BonusDamage / 100.f);
	}
};

// Ό³Έν :
class GameEngineCollision;
class GameEngineTextureRenderer;
class Player : public GameEngineActor
{
private:
	static Player* MainPlayer;

public:
	static Player* GetMainPlayer()
	{
		return MainPlayer;
	}

public:
	// constrcuter destructer
	Player();
	~Player();

	// delete Function
	Player(const Player& _Other) = delete;
	Player(Player&& _Other) noexcept = delete;
	Player& operator=(const Player& _Other) = delete;
	Player& operator=(Player&& _Other) noexcept = delete;


	void KeyBinding();
	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);

	void Attack(int _Direction);

	void PlayerSetPosition(float4 _NewPos);

	inline int GetHp() {
		return Hp;
	}

	inline int GetMaxHp() {
		return MaxHp;
	}

	void LoseHp(int _Value = 1);

	void Assault(int _Value = 1);
	void AddStuff(int _Type, int _Value = 1);

	int GetStuff(int _Type) {
		return Stuffs[_Type];
	}

	bool GetInvincible() {
		return Invincible;
	}

	float Damage;

	void LootItem();

	PlayerInfo& GetInfo() {
		return Info;
	}
protected:
	void Start() override;
	void CreateFrameAnimation();
	void StartAcceleration(int _Horizontal, int _Vertical);
	void StopAcceleration();
	void Update(float _DeltaTime);
	void End()  {}

	GameEngineTextureRenderer* HPRenderer;

	PlayerInfo Info;

	float4 Velocity;

	int Hp, MaxHp;

	int Stuffs[(int)StuffType::Heart];

	float4 Color;

	std::vector<GameEngineTextureRenderer*> AllPartsRenderer;

	float GetVertical();
	float GetHorizontal();

	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	void BlinkingThread(ULONGLONG _Duration);

	void LootStart(const StateInfo& _Info);
	void LootEnd(const StateInfo& _Info);
	void LootUpdate(float _DeltaTime, const StateInfo& _Info);

	void HitStart(const StateInfo& _Info);
	void HitEnd(const StateInfo& _Info);
	void HitUpdate(float _DeltaTime, const StateInfo& _Info);

	void HeadAttackStart(const StateInfo& _Info);

	void HeadIdleStart(const StateInfo& _Info);

	void HeadAttackUpdate(float _DeltaTime, const StateInfo& _Info);

	void HeadIdleUpdate(float _DeltaTime, const StateInfo& _Info);

	// void MoveStart(const StateInfo& _Info);
	void MoveUpdate(float _DeltaTime, const StateInfo& _Info);
	float GetAxis(float _Value);
private:
	std::shared_ptr<std::atomic<bool>> IsRunning;
	std::map<std::string, GameEngineTextureRenderer*> AllRenderer;

	float4 MoveDistance;
	bool IsAccel;
	float ElapsedTime;
	int HorizontalDir;
	int VerticalDir;
	float Accel, Decel;

	bool Invincible;
	float Speed;
	GameEngineStateManager StateManager, AttackManager;
	int HeadDirection;
	GameEngineFontRenderer* Font;
};
