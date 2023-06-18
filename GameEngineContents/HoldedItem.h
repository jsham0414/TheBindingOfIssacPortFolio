#pragma once
#include <GameEngineContents/ItemInfo.h>

class HoldedItem : public GameEngineActor {
public:
	static std::map<int, ItemInfo*> ItemInfos;
public:
	HoldedItem();
	~HoldedItem();

	// delete Function
	HoldedItem(const HoldedItem& _Other) = delete;
	HoldedItem(HoldedItem&& _Other) noexcept = delete;
	HoldedItem& operator=(const HoldedItem& _Other) = delete;
	HoldedItem& operator=(HoldedItem&& _Other) noexcept = delete;

	ItemInfo* Info;
	GameEngineStateManager StateManager;
protected:
	void Start() override;
	void Update(float _DeltaTime);
	void End() {}

	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

	void LootStart(const StateInfo& _Info);
	void LootUpdate(float _DeltaTime, const StateInfo& _Info);

	void SetRandomItem();

private:
	float LastTime;
};

