#pragma once
#include <GameEngineCore/CoreMinimal.h>
#include <GameEngineContents/Enums.h>
#include "../GameEngineCore/GameEngineLevel.h"
#include "HpInstance.h"
#include "StuffGuider.h"
#include "BossUI.h"


// Ό³Έν :
class HpInstance;
class GameEngineCollision;
class GameEngineTextureRenderer;
class UIManager : public GameEngineActor {
public:
	UIManager();
	~UIManager();

	// delete Function
	UIManager(const UIManager& _Other) = delete;
	UIManager(UIManager&& _Other) noexcept = delete;
	UIManager& operator=(const UIManager& _Other) = delete;
	UIManager& operator=(UIManager&& _Other) noexcept = delete;

	void SetHp(int _Hp);
	void SetStuff(int _Type, int _Value);

protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	LinkedList<HpInstance*> HpList;
	StuffGuider* Stuffs[static_cast<int>(StuffType::Heart)];
	
	//std::unordered_map<std::string, GameEngineTextureRenderer*> AllRenderer;
	BossUI* BossUIInstance;

	GameEngineTextureRenderer* HpRenderer, * BombRenderer, * CoinRenderer, * KeyRenderer;
};

