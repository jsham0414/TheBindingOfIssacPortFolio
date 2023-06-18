#include "PreCompile.h"
#include "Bullet.h"
#include <GameEngineCore/GameEngineDefaultRenderer.h>


Bullet::Bullet() {
	Init();
}

Bullet::~Bullet() {
}

void Bullet::Init() {
	Direction = -1;
	Lean = -0.2f;
	StartPosition = 0;
	Distance = 1000;
	Time = 0.f;
	Speed = 350.f;
}

void Bullet::SetDirection(int _Direction) {
	Direction = _Direction;
	
	float NewAngle = 0;
	switch (Direction) {
	case 2:
		NewAngle = 270;
		break;
	case 4:
		NewAngle = 180;
		break;
	case 6:
		NewAngle = 0;
		break;
	case 8:
		NewAngle = 90;
		break;
	}
	SetAngle(NewAngle);
	
	StateManager.ChangeState("Idle");
}

void Bullet::CreateFrameAnimation() {
	Renderer->CreateFrameAnimationCutTexture("Idle", FrameAnimation_DESC("Tears.png", 5, 5, 0.1f, false));
	Renderer->CreateFrameAnimationCutTexture("Dead", FrameAnimation_DESC("effect_015_tearpoofb.png", 0, 15, 0.03f, false));
}

void Bullet::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	CreateFrameAnimation();
	//Renderer->ChangeFrameAnimation("Idle");
	Renderer->SetTexture("Tears.png", 5);
	Renderer->ScaleToCutTexture();

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalScale(Renderer->GetTransform().GetWorldScale() / 3.f);
	Collision->ChangeOrder(OBJECTORDER::Projectile);
	Collision->SetMass(100.f);

	StateManager.CreateStateMember("Idle"
		, std::bind(&Bullet::IdleStateUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Bullet::IdleStateStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Dead"
		, std::bind(&Bullet::DeathStateUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Bullet::DeathStateStart, this, std::placeholders::_1)
	);
}

void Bullet::Update(float _DeltaTime) {
	if (StateManager.GetCurStateName() != "")
		StateManager.Update(_DeltaTime);
}

bool Bullet::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	if (_This->GetOrder() == _Other->GetOrder()) {
		return false;
	}

	Player* _Player = static_cast<Player*>(_Other->GetActor());

	//TODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODOTODO
	if (_Other->GetOrder() == (int)OBJECTORDER::Monster) {
		auto Infos = static_cast<Monster*>(_Other->GetActor());
		float DamageValue = Player::GetMainPlayer()->GetInfo().GetDamage();
		Infos->Damage(DamageValue);
	} else if (_Other->GetOrder() == (int)OBJECTORDER::Player) {
		_Player->Assault();
	}

	//_Other->GetActor()->Death();
	StateManager.ChangeState("Dead");
	return true;
}

void Bullet::IdleStateStart(const StateInfo& _Info) {
	Renderer->SetScaleRatio(1.0f);

	float4 Position = GetTransform().GetWorldPosition();
	StartPos = GetTransform().GetWorldPosition();

	float xx = Distance * cosf(Angle * (GameEngineMath::PI / 180.f));
	float yy = Distance * sinf(Angle * (GameEngineMath::PI / 180.f));
	EndPos = { xx, yy, 0, 0 };

	Gravity = 200.f;
	Velocity = 50.f;
	Accumulate = 0.f;

	StartPosition = Position.y;
}

void Bullet::IdleStateUpdate(float _DeltaTime, const StateInfo& _Info) {
	Velocity -= Gravity * _DeltaTime;
	Accumulate += Velocity * _DeltaTime;
	GetTransform().SetLocalMove(float4::UP * Velocity * _DeltaTime);

	GetTransform().SetWorldMove(GetTransform().GetRightVector() * Speed * _DeltaTime);

	Renderer->SetScaleRatio(2.f);
	Renderer->ChangeFrameAnimation("Idle");
	Renderer->ScaleToCutTexture();

	if (Velocity <= 0 && Accumulate < -30.f) {
		StateManager.ChangeState("Dead");
		return;
	}

	OBJECTORDER Order = static_cast<OBJECTORDER>(GetOrder());
	Order = Order == OBJECTORDER::Monster ? OBJECTORDER::Player : OBJECTORDER::Monster;
	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, Order, CollisionType::CT_AABB2D,
		std::bind(&Bullet::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2)
	);

	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Bomb, CollisionType::CT_AABB2D,
		std::bind(&Bullet::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2)
	);

	Collision->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::AirWall, CollisionType::CT_OBB2D,
		std::bind(&Bullet::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void Bullet::DeathStateStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Dead");
	Renderer->SetScaleRatio(1.5f);
	Renderer->ScaleToCutTexture();
	Renderer->GetTransform().SetLocalRotation(float4(0, 0, rand() % 360));
	GetTransform().StopPhysics();
}

void Bullet::DeathStateUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Renderer->CurAnimationEnd()) {
		Death();
		StateManager.ChangeState("Idle");
	}
}