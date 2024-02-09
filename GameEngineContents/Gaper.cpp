#include "PreCompile.h"
#include "Gaper.h"
#include "MapManager.h"
#include <GameEngineContents/DamageBlood.h>

Gaper::Gaper() {
	MoveSpeed = 150.f;
	Hp = 15.f;
}

Gaper::~Gaper() {
}

void Gaper::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->GetTransform().SetLocalPosition({ 0.f, 0.f, 11.f });
	auto Head = CreateComponent<GameEngineTextureRenderer>();

	Head->SetTexture("monster_017_gaper.png", 1);
	Head->ScaleToCutTexture(1);
	Head->GetTransform().SetLocalPosition({ 0.f, 22.f, 10.f });

	GaperRenderer["Head"] = Head;
	GaperRenderer["Body"] = Renderer;

	CreateFrameAnimation();

	GaperRenderer["Body"]->ChangeFrameAnimation("Down");
	Direction = 2;
	GaperRenderer["Body"]->ScaleToCutTexture(1);

	Collision = CreateComponent<GameEngineCollision>();
	Collision->ChangeOrder(OBJECTORDER::Monster);
	Collision->GetTransform().SetLocalScale({ 40, 40 });
}

void Gaper::Update(float _DeltaTime) {
	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, (int)OBJECTORDER::Player, CollisionType::CT_AABB2D,
		std::bind(&Gaper::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2));
	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, (int)OBJECTORDER::Monster, CollisionType::CT_AABB2D);

	if (LastTime > 0.25f) {
		auto Blood = GetLevel()->CreateActor<DamageBlood>();
		float4 NewPos = GetTransformData().WorldPosition;
		NewPos.z = 2000;
		Blood->GetTransform().SetWorldPosition(NewPos);
		LastTime = 0.f;
	}


	float Dist = (Player::GetMainPlayer()->GetTransformData().WorldPosition - GetTransformData().WorldPosition).Length2D();
	float4 Dir = (Player::GetMainPlayer()->GetTransformData().WorldPosition - GetTransformData().WorldPosition).NormalizeReturn();
	if (Dist > 1.f) {
		float4 Greatest = { 2, Dir.y };
		if (abs(Dir.x) > abs(Greatest.y)) {
			Greatest.y = Dir.x;
			if (Dir.x > 0) {
				Greatest.x = 6;
			} else {
				Greatest.x = 4;
			}
		}
		if (abs(Dir.y) > abs(Greatest.y)) {
			Greatest.y = Dir.y;
			if (Dir.y > 0) {
				Greatest.x = 8;
			} else {
				Greatest.x = 2;
			}
		}

		if (Greatest.x == 4) {
			GaperRenderer["Body"]->ChangeFrameAnimation("Left");
		}
		if (Greatest.x == 6) {
			GaperRenderer["Body"]->ChangeFrameAnimation("Right");
		} 
		if (Greatest.x == 8) {
			GaperRenderer["Body"]->ChangeFrameAnimation("Up");
		} 
		if (Greatest.x == 2) {
			GaperRenderer["Body"]->ChangeFrameAnimation("Down");
		}
		GetTransform().SetWorldMove(Dir * MoveSpeed * _DeltaTime);
	}

	LastTime += _DeltaTime;
}

bool Gaper::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	if (_This->GetOrder() == _Other->GetOrder()) {
		return false;
	}

	if (_Other->GetOrder() == (int)OBJECTORDER::Player) {
		Player* _Player = static_cast<Player*>(_Other->GetActor());
		if (_Player->GetInvincible())
			return false;

		_Player->Assault();
	}

	return true;
}

void Gaper::CreateFrameAnimation() {

	GaperRenderer["Body"]->CreateFrameAnimationCutTexture("Down", FrameAnimation_DESC("issac_body_vertical.png", 0, 9, 0.1f));
	GaperRenderer["Body"]->CreateFrameAnimationCutTexture("Up", FrameAnimation_DESC("issac_body_vertical.png", 9, 0, 0.1f));
	GaperRenderer["Body"]->CreateFrameAnimationCutTexture("Left", FrameAnimation_DESC("issac_body_horizontal.png", 0, 7, 0.1f));
	GaperRenderer["Body"]->CreateFrameAnimationCutTexture("Right", FrameAnimation_DESC("issac_body_horizontal.png", 8, 15, 0.1f));
}
