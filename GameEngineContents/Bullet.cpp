#include "PreCompile.h"
#include "Bullet.h"
#include <GameEngineCore/GameEngineDefaultRenderer.h>

Bullet::Bullet() : Speed(350.0f) {
	Direction = -1;
	Lean = -0.2f;
	StartPosition = 0;
	Distance = 1000;
	Time = 0.f;
}

Bullet::~Bullet() {
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
}

void Bullet::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->GetTransform().SetWorldScale({ 20, 20, 1 }); // 24
	if (Renderer->GetCurTexture() == nullptr)
		Renderer->SetTexture("Tear.png");

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalScale({ 20.0f, 20.0f, 1.0f });
	Collision->ChangeOrder(OBJECTORDER::Projectile);

	StateManager.CreateStateMember("Idle"
		, std::bind(&Bullet::IdleStateUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Bullet::IdleStateStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Death"
		, std::bind(&Bullet::DeathStateUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Bullet::DeathStateStart, this, std::placeholders::_1)
	);
}

void Bullet::Update(float _DeltaTime) {
	if (StateManager.GetCurStateStateName() != "")
		StateManager.Update(_DeltaTime);
}

bool Bullet::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	if (_This->GetOrder() == _Other->GetOrder()) {
		return false;
	}

	//_Other->GetActor()->Death();
	StateManager.ChangeState("Death");
	return true;
}

void Bullet::IdleStateStart(const StateInfo& _Info) {
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
	GetTransform().SetWorldMove(float4::UP * Velocity * _DeltaTime);
	GetTransform().SetWorldMove(GetTransform().GetRightVector() * Speed * _DeltaTime);

	if (Velocity <= 0 && Accumulate < -30.f) {
		StateManager.ChangeState("Death");
		return;
	}

	OBJECTORDER Order = static_cast<OBJECTORDER>(GetOrder());
	Order = Order == OBJECTORDER::Monster ? OBJECTORDER::Player : OBJECTORDER::Monster;
	Collision->IsCollision(CollisionType::CT_SPHERE, Order, CollisionType::CT_SPHERE,
		std::bind(&Bullet::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void Bullet::DeathStateStart(const StateInfo& _Info) {
	Death();
}

void Bullet::DeathStateUpdate(float _DeltaTime, const StateInfo& _Info) {
}