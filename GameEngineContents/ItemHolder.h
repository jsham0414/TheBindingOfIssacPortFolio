#pragma once
#include <GameEngineContents/HoldedItem.h>

class ItemHolder : public GameEngineActor {
public:
	// constrcuter destructer
	ItemHolder();
	~ItemHolder();

	// delete Function
	ItemHolder(const ItemHolder& _Other) = delete;
	ItemHolder(ItemHolder&& _Other) noexcept = delete;
	ItemHolder& operator=(const ItemHolder& _Other) = delete;
	ItemHolder& operator=(ItemHolder&& _Other) noexcept = delete;
protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}
	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);

	std::thread ColorChange;
private:
	bool Generated;

	HoldedItem* Item;

	GameEngineStateManager StateManager;
};

