#include "PreCompile.h"
#include "HoldedItem.h"
#include <GameEngineCore/jsoncpp/json/json.h>
#include <GameEngineBase/GameEngineDirectory.h>
#include <fstream>
#include "UIManager.h"
#include <GameEngineContents/PopUpMessage.h>

std::map<int, ItemInfo*> HoldedItem::ItemInfos;

class HoldedItemInit {
public:
	int Count;
public:
	HoldedItemInit() {
		GameEngineDirectory Dir;
		Dir.MoveParentToExitsChildDirectory("GameEngineData");
		Dir.Move("GameEngineData");
		Dir.Move("Item");

		Json::Value Root;
		std::ifstream TempFile(Dir.PlusFilePath("ItemData.json"));
		//Json::CharReaderBuilder Reader;
		Json::String Error;
		//auto StreamParser = Json::parseFromStream(Reader, TempFile, &Root, &Error);


		Json::Reader Reader;
		Reader.parse(TempFile, Root);

		TempFile.close();

		Count = 0;
		while (true) {
			if (Root[to_string(Count)].empty())
				break;

			Json::Value Info = Root[to_string(Count)];

			AddItem(Count, Info["Name"].asString(), Info["Description"].asString(), Info["Icon"].asString(), Info["Costume"].asString());

			Count++;
		}


	}

	~HoldedItemInit() {
		for (std::map<int, ItemInfo*>::iterator Iter = HoldedItem::ItemInfos.begin(); Iter != HoldedItem::ItemInfos.end(); Iter++) {
			delete (Iter->second);
		}
		HoldedItem::ItemInfos.clear();
	}

	ItemInfo* AddItem(int _Count, std::string _Name, std::string _Description, std::string _Icon, std::string _Costume) {
		auto Item = new ItemInfo(_Name, _Description, _Icon, _Costume);
		Item->Index = _Count;
		HoldedItem::ItemInfos.insert(std::make_pair(_Count, Item));
		return Item;
	}
};

HoldedItemInit Inst;

HoldedItem::HoldedItem() {
	LastTime = 0.f;
}

HoldedItem::~HoldedItem() {
}

void HoldedItem::Start() {
	SetRandomItem();
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	if (GameEngineTexture::Find(Info->Icon) != nullptr) {
		Renderer->SetTexture(Info->Icon);
	}
	Renderer->ScaleToTexture();

	Renderer->GetTransform().SetLocalPosition({ -2.5f, 10.f });

	StateManager.CreateStateMember("Idle"
		, std::bind(&HoldedItem::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&HoldedItem::IdleStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Loot"
		, std::bind(&HoldedItem::LootUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&HoldedItem::LootStart, this, std::placeholders::_1)
	);

	StateManager.ChangeState("Idle");
}

void HoldedItem::Update(float _DeltaTime) {
	StateManager.Update(_DeltaTime);
}

void HoldedItem::IdleStart(const StateInfo& _Info) {

}

void HoldedItem::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = LastTime * 2.f;

	if (Duration < 1.f) {
		float NewY = std::lerp(0.f, 10.f, GameEngineMath::EaseOutQuint(fmin(1.f, Duration)));
		Renderer->GetTransform().SetLocalPosition({ -2.5f, 10.f + NewY });
	} else {
		Duration -= 1.f;
		float NewY = std::lerp(10.f, 0.f, GameEngineMath::EaseInQuint(fmin(1.f, Duration)));
		Renderer->GetTransform().SetLocalPosition({ -2.5f, 10.f + NewY });

		if (Duration >= 2.f)
			LastTime = 0.f;
	}

	LastTime += _DeltaTime;
}

void HoldedItem::LootStart(const StateInfo& _Info) {
	Player* _Player = Player::GetMainPlayer();

	PopUpMessage* Message = GetLevel()->CreateActor<PopUpMessage>();
	Message->SetTitle(Info->Name);
	Message->SetText(Info->Description);
	Message->Play();


	switch (Info->Index) {
	case 0:
		_Player->GetInfo().BonusDamage += 10.f;
		break;
	case 1:
		_Player->GetInfo().MaxHP += 2;
		_Player->LoseHp(-2);
		break;
	case 2:
		_Player->GetInfo().AttackSpeed += 10.f;
		break;
	}
	
	SetParent(_Player);
	GetTransform().SetLocalPosition({ 0.f, 40.f });
	Death(1.f);
}

void HoldedItem::LootUpdate(float _DeltaTime, const StateInfo& _Info) {

}

void HoldedItem::SetRandomItem() {
	int i = rand() % ItemInfos.size();
	Info = ItemInfos[i];
}
