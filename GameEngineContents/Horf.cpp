#include "PreCompile.h"
#include "Horf.h"
#include "Bullet.h"
#include "Player.h"

Horf::Horf() :
	ShootOnce(false) {
	SetName("Horf");
	Power = 2.f;
	Period = 1.8f;
	AttackDelay = 1500;
	Hp = 10;
}

Horf::~Horf() {
}

static float4 HorfScale = { 0 };

void Horf::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->SetScaleMode(SCALEMODE::IMAGE);

	CreateFrameAnimation();

	Renderer->ChangeFrameAnimation("Idle");
	StateManager.CreateStateMember("Idle"
		, std::bind(&Horf::IdleStateUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Horf::IdleStateStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Attack"
		, std::bind(&Horf::AttackStateUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Horf::AttackStateStart, this, std::placeholders::_1)
	);
	StateManager.ChangeState("Idle");

	Renderer->ScaleToCutTexture(0);

	Renderer->SetPivotToVector(float4(0.0f, 0.48f, 0.0f, 0.0f));

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalPosition({ 0, 25, 0 });
	Collision->GetTransform().SetWorldScale({ 40.0f, 40.0f, 50.0f });
	Collision->ChangeOrder(OBJECTORDER::Monster);
	float4 Color = float4::RED;
	Color.a = 0.2f;
	Collision->SetDebugSetting(CollisionType::CT_AABB, Color);

	GameEngineActorInfo* ActorInfo = CreateComponent<GameEngineActorInfo>();
	ActorInfo->GetInfo().SetMaxHp(10.f);

	HorfScale = Renderer->GetTransform().GetWorldScale();
}

void Horf::Update(float _DeltaTime) {
	StateManager.Update(_DeltaTime);

	float x = Power * cosf((2 * GameEngineMath::PI * (GetTickCount64() / 50.f)) / Period);
	Renderer->GetTransform().SetLocalPosition({x, 0.f, 0.0f});
}

bool Horf::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	return false;
}

void Horf::CreateFrameAnimation() {
	Renderer->CreateFrameAnimationCutTexture("Idle", FrameAnimation_DESC("Horf.png", 0, 0, 0.3f));
	Renderer->CreateFrameAnimationCutTexture("Attack", FrameAnimation_DESC("Horf.png", 0, 2, 0.25f));
}

void Horf::Damage() {
	ColorChange = thread(&Monster::DamageAnimation, this);
	ColorChange.detach();
}

void Horf::IdleStateStart(const StateInfo& _Info) {
	Power = 2.5f;
	Renderer->ChangeFrameAnimation("Idle");
	Renderer->GetTransform().SetWorldScale({ 32 * 1.5f, 32 * 1.5f, 1 });
}

void Horf::IdleStateUpdate(float _DeltaTime, const StateInfo& _Info) {
	float4 PlayerPosition = Player::GetMainPlayer()->GetTransform().GetWorldPosition();
	float4 Position = GetTransform().GetWorldPosition();
	
	float Distance = (PlayerPosition - Position).Length2D();

	if (Distance < 250.f) {
		if (LastDelay < GetTickCount64()) {
			LastDelay = GetTickCount64() + AttackDelay;
			StateManager.ChangeState("Attack");
		}
	}
}

void Horf::AttackStateStart(const StateInfo& _Info) {
	ShootOnce = false;
	Power = 1.0f;
	Size = 1.f;
	Renderer->ChangeFrameAnimation("Attack");
}

void Horf::ShootBullet() {
}

void Horf::AttackStateUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Renderer->CurAnimationEnd()) {
		StateManager.ChangeState("Idle");
		return;
	}

	Size += 5.f * _DeltaTime;
	float y = powf(10.f * cosf(GameEngineMath::PI * Size * 2.f), 3.f);
	y = max(y, 0);

	Renderer->GetTransform().SetWorldScale({ HorfScale.x * 1.5f - (y * 50.f), HorfScale.y * 1.5f + y * 100.f, 1 });

	if (Renderer->CurAnimationFrameIndex() > 1 && !ShootOnce) {
		ShootOnce = true;

		float4 PlayerPosition = Player::GetMainPlayer()->GetTransform().GetWorldPosition();
		float4 Position = GetTransform().GetWorldPosition();

		Bullet* NewBullet = GameEngineCore::GetCurLevel()->CreateActor<Bullet>(Collision->GetOrder());

		float Angle = atan2f(PlayerPosition.y - Position.y, PlayerPosition.x - Position.x) * GameEngineMath::RadianToDegree;

		NewBullet->SetAngle(Angle);
		float4 pos = GetTransform().GetWorldPosition();
		pos.y += 0.f;
		NewBullet->GetTransform().SetWorldPosition(pos);
		NewBullet->GetRenderer()->SetTexture("Tear_002.png");
		NewBullet->StateManager.ChangeState("Idle");
	}
}
