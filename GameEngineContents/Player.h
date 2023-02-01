#pragma once
#include <GameEngineCore/CoreMinimal.h>
#include "Enums.h"

// ���� :
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

	void KeyBinding();

	// delete Function
	Player(const Player& _Other) = delete;
	Player(Player&& _Other) noexcept = delete;
	Player& operator=(const Player& _Other) = delete;
	Player& operator=(Player&& _Other) noexcept = delete;

	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);

	void Attack(int _Direction);

	inline int GetHp() {
		return Hp;
	}

	inline int GetMaxHp() {
		return MaxHp;
	}

	void LoseHp(int _Value = 1) {
		Hp -= _Value;
	}

	int GetStuff(int _Type) {
		return Stuffs[_Type];
	}

protected:
	void Start() override;
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End()  {}

	GameEngineTextureRenderer* HeadRenderer, *BodyRenderer;
	GameEngineTextureRenderer* HPRenderer;

	float PlayerHorizontal, PlayerVertical;
	float Slope;
	float Accel;

	int Hp, MaxHp;

	int Stuffs[(int)StuffType::End];

	float4 Color;

	std::vector<GameEngineTextureRenderer*> AllPartsRenderer;

	void IdleStart(const StateInfo& _Info);
	float GetVertical();
	float GetHorizontal();
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	void HeadAttackStart(const StateInfo& _Info);

	void HeadIdleStart(const StateInfo& _Info);

	void HeadAttackUpdate(float _DeltaTime, const StateInfo& _Info);

	void HeadIdleUpdate(float _DeltaTime, const StateInfo& _Info);

	// void MoveStart(const StateInfo& _Info);
	void MoveUpdate(float _DeltaTime, const StateInfo& _Info);

private:
	float Speed;
	GameEngineStateManager StateManager, AttackManager;
	int HeadDirection;
	GameEngineFontRenderer* Font;
};


//class �ܻ� : public GameEngineActor
//{
//	std::vector<GameEngineTextureRenderer*> AllPartsRenderer;
//
//public:
//	void Start() 
//	{
//		Death(0.2f);
//
//		// 
//		//HPRenderer->renderOption.Option00 = 1;
//	}
//
//	�ؽ�ó����(Player* _Player) 
//	{
//		for (size_t i = 0; i < _Player->AllPartsRenderer.size(); i++)
//		{
//			AllPartsRenderer[i]->SetTexture(_Player->AllPartsRenderer[i]->GetCurTexture());
//		}
//	}
//
//	������Ʈ() 
//	{
//		for (size_t i = 0; i < _Player->AllPartsRenderer.size(); i++)
//		{
//			AllPartsRenderer[i]->GetPixelData().PlusColor.a -= ��ŸŸ��;
//		}
//
//		// ���� 
//	}
//};