#include "PreCompile.h"
#include "DropItem.h"

DropItem::DropItem() {
}

DropItem::~DropItem() {
}

void DropItem::SetType(int _Type) {
	Type = _Type;

	switch ((StuffType)Type) {
	case StuffType::Bomb:
		Renderer->CreateFrameAnimationCutTexture("Start", FrameAnimation_DESC("pickup_016_bomb.png", 0, 0, 0.f));
		Renderer->CreateFrameAnimationCutTexture("Idle", FrameAnimation_DESC("pickup_016_bomb.png", 0, 0, 0.f));
		Renderer->CreateFrameAnimationCutTexture("Dead", FrameAnimation_DESC("pickup_016_bomb.png", 0, 0, 0.f));
		break;
	case StuffType::Gold:
		Renderer->CreateFrameAnimationCutTexture("Start", FrameAnimation_DESC("pickup_002_coin.png", 8, 15, 0.05f, false));
		Renderer->CreateFrameAnimationCutTexture("Idle", FrameAnimation_DESC("pickup_002_coin.png", 0, 5, 0.08f));
		Renderer->CreateFrameAnimationCutTexture("Dead", FrameAnimation_DESC("pickup_002_coin.png", 16, 23, 0.05f, false));
		break;
	case StuffType::Key:
		Renderer->CreateFrameAnimationCutTexture("Start", FrameAnimation_DESC("pickup_003_key.png", 0, 0, 0.f));
		Renderer->CreateFrameAnimationCutTexture("Idle", FrameAnimation_DESC("pickup_003_key.png", 0, 0, 0.f));
		Renderer->CreateFrameAnimationCutTexture("Dead", FrameAnimation_DESC("pickup_003_key.png", 0, 0, 0.f));
		break;
	case StuffType::Heart:
		Renderer->CreateFrameAnimationCutTexture("Start", FrameAnimation_DESC("pickup_001_heart.png", 0, 0, 0.f));
		Renderer->CreateFrameAnimationCutTexture("Idle", FrameAnimation_DESC("pickup_001_heart.png", 0, 0, 0.f));
		Renderer->CreateFrameAnimationCutTexture("Dead", FrameAnimation_DESC("pickup_001_heart.png", 0, 0, 0.f));
		break;
	default:
		return;
	}

	StateManager.ChangeState("Init");
}

void DropItem::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalScale({ 20.f, 20.f });


	StateManager.CreateStateMember("Init"
		, std::bind(&DropItem::InitUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&DropItem::InitStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Idle"
		, std::bind(&DropItem::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&DropItem::IdleStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Dead"
		, std::bind(&DropItem::DeadUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&DropItem::DeadStart, this, std::placeholders::_1)
	);
}

void DropItem::Update(float _DeltaTime) {
	StateManager.Update(_DeltaTime);
}

void DropItem::InitStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Start");
	Renderer->ScaleToCutTexture();
}

void DropItem::InitUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Renderer->CurAnimationEnd())
		StateManager.ChangeState("Idle");
}

void DropItem::IdleStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Idle");
}

void DropItem::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D,
		std::bind(&DropItem::PlayerCollision, this, std::placeholders::_1, std::placeholders::_2))) {
		StateManager.ChangeState("Dead");
	}
}

void DropItem::DeadStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Dead");
	GetTransform().StopPhysics();
}

void DropItem::DeadUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Renderer->CurAnimationEnd())
		Death();
}

bool DropItem::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	auto& Info = Player::GetMainPlayer()->GetInfo();

	switch ((StuffType)Type) {
	case StuffType::Gold:
	case StuffType::Bomb:
	case StuffType::Key:
		Player::GetMainPlayer()->AddStuff(Type);
		break;
	case StuffType::Heart:
		if (Info.MaxHP == Info.HP)
			return false;
		Player::GetMainPlayer()->LoseHp(-1);
		break;
	}

	return true;
}
