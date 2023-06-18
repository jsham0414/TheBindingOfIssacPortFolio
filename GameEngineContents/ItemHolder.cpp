#include "PreCompile.h"
#include "ItemHolder.h"

enum ItemHolderType {
	NORMAL,
	ARCADE_0 = 3,
	ARCADE_1,
	STONE,
	ARCADE_2,
	GOLD,
	RED,
};

ItemHolder::ItemHolder() {
	SetName("ItemHolder");
	Generated = false;
}

ItemHolder::~ItemHolder() {
}

void ItemHolder::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->SetTexture("levelitem_001_itemaltar.png", ItemHolderType::NORMAL);
	Renderer->ScaleToCutTexture();

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalScale({ 40, 20 });
	Collision->GetTransform().SetLocalPosition({0.f, -15.f});
	Collision->GetTransform().SetStatic(true);
}

void ItemHolder::Update(float _DeltaTime) {
	if (Item != nullptr) {
		Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D,
			std::bind(&ItemHolder::PlayerCollision, this, std::placeholders::_1, std::placeholders::_2)
		);
	}

	if (Generated)
		return;

	Generated = true;
	Item = GetLevel()->CreateActor<HoldedItem>();
	float4 NewPos = GetTransformData().WorldPosition;
	NewPos.z -= 100;
	Item->GetTransform().SetWorldPosition(NewPos);
}

bool ItemHolder::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	//Item->DetachObject();
	Item->StateManager.ChangeState("Loot");
	Item = nullptr;

	Player::GetMainPlayer()->LootItem();

	return false;
}
