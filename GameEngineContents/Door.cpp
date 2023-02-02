#include "PreCompile.h"
#include "Door.h"
#include <GameEngineCore/GameEngineTextureRenderer.h>

Door::Door() {
	SetName("Door");
}

Door::~Door() {
	SAFE_DELETE(Spawn);
}

void Door::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->GetTransform().SetWorldScale({ 98.f * 1.f, 76.f * 1.f, 1 });
	//Renderer->GetTransform().SetLocalPosition({ 0.0f, 9.f, 10.0f });

	Renderer->SetTexture("WoodDoor.png", 0);

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetWorldScale(float4(Renderer->GetTransformData().WorldScaling.y, Renderer->GetTransformData().WorldScaling.y));
	float4 Color = float4::RED;
	Color.a = 0.2f;
	Collision->SetDebugSetting(CollisionType::CT_AABB2D, Color);
	Collision->GetTransform().SetLocalPosition({ 0, 0 });

	Collision->GetTransform().SetStatic(true);
	CreateFrameAnimation();

	StateManager.CreateStateMember("Idle"
		, std::bind(&Door::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Door::IdleStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Battle"
		, std::bind(&Door::BattleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Door::BattleStart, this, std::placeholders::_1)
	);

	StateManager.ChangeState("Idle");
}

void Door::Initialize() {
	Spawn = new GameEngineTransform();
	Spawn->SetParentTransform(GetTransform());
	Spawn->SetLocalPosition({ 0, -40 });
}

bool Door::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {


	return true;
}

void Door::CreateFrameAnimation() {
	Renderer->CreateFrameAnimationCutTexture("Open", FrameAnimation_DESC("WoodDoor.png", 0, 3, 0.1f, false));
	Renderer->CreateFrameAnimationCutTexture("Close", FrameAnimation_DESC("WoodDoor.png", 3, 0, 0.1f, false));

}

void Door::Update(float _DeltaTime) {

}

void Door::SetType(GameRoomType _Type) {
	Type = _Type;
}

void Door::BattleStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Close");
	Collision->GetTransform().SetLocalPosition({ 0, 0 });
}

void Door::IdleStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Open");
	Collision->GetTransform().SetLocalPosition({ 0, 40 });
}

void Door::BattleUpdate(float _DeltaTime, const StateInfo& _Info) {
}

void Door::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
}
