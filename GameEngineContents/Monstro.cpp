#include "PreCompile.h"
#include "Monstro.h"
#include "Bullet.h"
#include "MapManager.h"
#include <GameEngineContents/DyingBlood.h>

Monstro::Monstro() :
	BigJumpTime(1.5f),
	JumpWidth(500.f) {
	SetName("Monstro");
	Speed = 300.f;
	BloodType = GameBloodType::HUGE;
	MaxHp = 60.f;
	Hp = MaxHp;
	Dead = false;
}

Monstro::~Monstro() {
}

void Monstro::Damage(float _Value) {
	Hp -= _Value;

	float f = (float)Hp / (float)MaxHp;
	if (BossUIInstance)
		BossUIInstance->GetRenderer()->GetPixelData().Slice.x = f;

	if (Hp <= 0 && !IsDeath() && StateManager.GetCurStateName() != "Dead") {
		StateManager.ChangeState("Dead");
		return;
	}

	auto ColorChange = thread(&Monster::DamageAnimation, this);
	ColorChange.detach();
}

void Monstro::Start() {
	GetTransform().SetLocalPosition({ 0.f, -100.f });

	auto Shadow = CreateComponent<GameEngineTextureRenderer>();
	Shadow->SetName("Shadow");
	Shadow->SetTexture("shadow.png");
	Shadow->GetPixelData().MulColor.a = 0.2f;
	Shadow->SetScaleRatio(1.3f);
	Shadow->ScaleToTexture();
	Shadow->GetTransform().SetLocalPosition({ 0.f, 10.f, 12.0f });

	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->SetTexture("boss_004_monstro.png", 0);
	Renderer->ScaleToCutTexture();
	Renderer->SetPivot(PIVOTMODE::TOP);

	MonstroRenderer["Main"] = Renderer;
	MonstroRenderer["Shadow"] = Shadow;

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalPosition({ 0, 30, 0 });
	Collision->GetTransform().SetLocalScale({ 140.0f, 50.0f, 50.0f });
	Collision->ChangeOrder(OBJECTORDER::Monster);
	Collision->SetMass(100.f);
	float4 Color = float4::RED;
	Color.a = 0.2f;
	Collision->SetDebugSetting(CollisionType::CT_AABB2D, Color);

	CreateFrameAnimation();

	StateManager.CreateStateMember("Idle"
		, std::bind(&Monstro::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::IdleStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("BigJump"
		, std::bind(&Monstro::BigJumpUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::BigJumpStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("BigJumping"
		, std::bind(&Monstro::BigJumpingUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::BigJumpingStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("BigJumpEnd"
		, std::bind(&Monstro::BigJumpEndUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::BigJumpEndStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("Spit"
		, std::bind(&Monstro::SpitUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::SpitStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("Spiting"
		, std::bind(&Monstro::SpitingUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::SpitingStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("Jump"
		, std::bind(&Monstro::JumpUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::JumpStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("Dead"
		, std::bind(&Monstro::DeadUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Monstro::DeadStart, this, std::placeholders::_1)
	);

	StateManager.ChangeState("Idle");

	BossUIInstance = GetLevel()->CreateActor<BossUI>();
}

void Monstro::Update(float _DeltaTime) {
	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D,
		std::bind(&Monstro::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2)
	);

	StateManager.Update(_DeltaTime);
}

bool Monstro::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	if (_This->GetOrder() == _Other->GetOrder()) {
		return false;
	}

	if (_Other->GetOrder() == (int)OBJECTORDER::Player) {
		Player* _Player = static_cast<Player*>(_Other->GetActor());
		_Player->Assault();
	}

	return true;
}

void Monstro::CreateFrameAnimation() {
	std::string MainTexture = "boss_004_monstro.png";

	Renderer->CreateFrameAnimationCutTexture(
		"Laugh",
		FrameAnimation_DESC(MainTexture, 0, 1, 0.1f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"BigJumpPre",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{7}, 0.2f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"BigJumpStart",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{6}, 0.5f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"BigJumping",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{4}, 0.1f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"BigJumpFalling",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{4, 5}, 0.01f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"BigJumpEnd",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{8}, 2.f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"JumpFalling",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{4, 5}, 0.1f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"JumpEnd",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{8}, 2.f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"Spit",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{5, 1, 3}, 0.15f, false)
	);
	Renderer->CreateFrameAnimationCutTexture(
		"Dead",
		FrameAnimation_DESC(MainTexture, std::vector<UINT>{8}, 0.1f)
	);
}

void Monstro::OnEvent() {
	BossUIInstance->On();
}

void Monstro::OffEvent() {
	BossUIInstance->Off();
}

void Monstro::IdleStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Laugh");
}

void Monstro::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Renderer->CurAnimationEnd())
		RandomPatern();
}

void Monstro::BigJumpStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("BigJumpPre");
	Falling = false;
}

void Monstro::BigJumpUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Renderer->GetCurAni()->Compare("BigJumpPre")) {
		if (Renderer->CurAnimationEnd()) {
			StateManager.ChangeState("BigJumping");
			return;
		}
	}
}

void Monstro::BigJumpingStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("BigJumpStart");
	StartPos = GetTransformData().WorldPosition;
	LastPlayerPosition = Player::GetMainPlayer()->GetTransformData().WorldPosition;
	TextureSize = MonstroRenderer["Shadow"]->GetTransformData().LocalScaling;
}

void Monstro::BigJumpingUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = _Info.StateTime / 0.75f;
	float NewY = 0.f;
	float ShadowScale = 0.f;
	if (Falling) {
		NewY = std::lerp(JumpWidth, 0.f, GameEngineMath::EaseInQuint(fmin(1.f, Duration)));
		ShadowScale = std::lerp(0.6f, 1.f, GameEngineMath::EaseInQuint(fmin(1.f, Duration)));
	} else {
		NewY = std::lerp(0.f, JumpWidth, GameEngineMath::EaseOutQuint(fmin(1.f, Duration)));
		ShadowScale = std::lerp(1.f, 0.6f, GameEngineMath::EaseOutQuint(fmin(1.f, Duration)));
	}

	Renderer->GetTransform().SetLocalPosition({ 0.f, NewY, 10.f });

	auto Shadow = MonstroRenderer["Shadow"];
	Shadow->GetTransform().SetLocalScale(Shadow->GetCurTexture()->GetScale() * ShadowScale);

	if (Duration >= 0.25f && Collision->IsUpdate())
		Collision->Off();

	if (Duration < 1.f) {
		float4 NewDir = (LastPlayerPosition - GetTransformData().WorldPosition).NormalizeReturn();
		GetTransform().SetWorldMove(NewDir * Speed * _DeltaTime);
	}

	if (Duration >= 1.f ||
		((LastPlayerPosition - GetTransformData().WorldPosition).Length2D() < 1.f && Falling != true)) {
		if (Falling == false) {
			Falling = true; 
			StateManager.ResetStateTime();
			Renderer->ChangeFrameAnimation("BigJumpFalling");
			Renderer->CurAnimationPauseOn();
		} else {
			BigJumpAttack();
			StateManager.ChangeState("BigJumpEnd");
		}
	}
}

void Monstro::BigJumpEndStart(const StateInfo& _Info) {
	TextureSize = Renderer->GetCurTexture()->GetCutScale(0);
	Falling = false;
	Renderer->CurAnimationPauseOff();
	Collision->On();
}

void Monstro::BigJumpEndUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = _Info.StateTime / 0.5f;

	float NewX = std::lerp(Renderer->GetScaleRatio(), Renderer->GetScaleRatio() * 1.2f, GameEngineMath::EaseOutBack(fmin(1.f, Duration)));
	float NewY = std::lerp(Renderer->GetScaleRatio(), Renderer->GetScaleRatio() * 0.8f, GameEngineMath::EaseOutBack(fmin(1.f, Duration)));
	float4 TexScale;
	TexScale.x = Direction * TextureSize.x * NewX;
	TexScale.y = TextureSize.y * NewY;
	Renderer->GetTransform().SetLocalScale(TexScale);

	NewX = std::lerp(1.f, 1.5f, GameEngineMath::EaseOutBack(fmin(1.f, Duration)));
	auto Shadow = MonstroRenderer["Shadow"];
	TexScale.x = Shadow->GetCurTexture()->GetScale().x * NewX;
	TexScale.y = Shadow->GetCurTexture()->GetScale().y * NewX;
	Shadow->GetTransform().SetLocalScale(TexScale);

	if (Duration >= 1.f) {
		Renderer->ScaleToCutTexture();
		Shadow->ScaleToTexture();

		StateManager.ChangeState("Idle");
	}
}

void Monstro::SpitStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Spit");
}

void Monstro::SpitUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (_Info.StateTime) / 0.25f;
	float NewY = std::lerp(0.0f, 1.f, sinf(fmin(1.f, Duration)));
	Renderer->GetTransform().SetLocalScale({ Direction * TextureSize.x, TextureSize.y * NewY });
	Duration = (_Info.StateTime) * 2.f;
	float NewX = std::lerp(0.5f, 20.0f, Duration);
	Renderer->GetTransform().SetLocalPosition({ -10.f + powf(sinf(NewX), 2.f) * 20.f, 0.f, 10.f });

	if (_Info.StateTime > 0.5f) {
		for (int i = 0; i < 6; i++) {
			float4 PlayerPosition = Player::GetMainPlayer()->GetTransform().GetWorldPosition();
			float4 Position = GetTransform().GetWorldPosition();

			float Angle = atan2f(PlayerPosition.y - Position.y, PlayerPosition.x - Position.x) * GameEngineMath::RadianToDegree - 30.f + (rand() % 60);

			Bullet* NewBullet = GetLevel()->CreateActor<Bullet>(Collision->GetOrder());
			NewBullet->Init();
			NewBullet->SetAngle(Angle);

			float BulletSpeed = (rand() % 200) + 200.f;
			NewBullet->SetSpeed(BulletSpeed);

			float4 NewPos = GetTransform().GetWorldPosition();
			NewPos.y += 0.f;
			NewBullet->GetTransform().SetWorldPosition(NewPos);
			NewBullet->StateManager.ChangeState("Idle");
		}
		StateManager.ChangeState("Spiting");
	}
}

void Monstro::SpitingStart(const StateInfo& _Info) {
	Renderer->GetTransform().SetLocalPosition({ 0.f, 0.f, 10.f });
}

void Monstro::SpitingUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (_Info.StateTime > 0.5f) {
		StateManager.ChangeState("Idle"); // 3가지 상태 중 뭐 할지 정하기
	}
}

void Monstro::DeadStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Dead");
}

void Monstro::DeadUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (_Info.StateTime) * 2.f;
	float NewX = std::lerp(0.5f, 20.0f, Duration);
	Renderer->GetTransform().SetLocalPosition({ -10.f + powf(sinf(NewX), 2.f) * 20.f, 0.f, 10.f });

	if (_Info.StateTime > 5.f) {
		auto Blood = GetLevel()->CreateActor<DyingBlood>();
		Blood->GetTransform().SetWorldPosition(GetTransformData().WorldPosition);
		Blood->SetType(BloodType);
		Off();
		GetLevel()->GetMapManager()->ObjectDestory(this);
	}
}

void Monstro::JumpStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("BigJumpPre");
	LastPlayerPosition = Player::GetMainPlayer()->GetTransform().GetWorldPosition();
}

void Monstro::JumpUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = _Info.StateTime / (BigJumpTime / 3.f);
	float NewY = 0.f;
	float ShadowScale = 0.f;
	if (Duration > 0.70f) {
		NewY = std::lerp(150.f, 0.f, GameEngineMath::EaseInQuint(fmin(1.f, Duration)));
		ShadowScale = std::lerp(0.8f, 1.f, GameEngineMath::EaseInQuint(fmin(1.f, Duration)));
	} else {
		NewY = std::lerp(0.f, 150.f, GameEngineMath::EaseOutQuint(fmin(1.f, Duration)));
		ShadowScale = std::lerp(1.f, 0.8f, GameEngineMath::EaseOutQuint(fmin(1.f, Duration)));
	}

	Renderer->GetTransform().SetLocalPosition({ 0.f, NewY, 10.f });

	auto Shadow = MonstroRenderer["Shadow"];
	Shadow->GetTransform().SetLocalScale(Shadow->GetCurTexture()->GetScale() * ShadowScale);

	if (Duration >= 0.25f && Collision->IsUpdate())
		Collision->Off();

	if (Duration <= 1.f) {
		float4 NewDir = (LastPlayerPosition - GetTransformData().WorldPosition).NormalizeReturn();
		GetTransform().SetWorldMove(NewDir * 200.f * _DeltaTime);
	}

	if (Duration >= 0.70f && Falling == false) {
		Falling = true;
		Renderer->ChangeFrameAnimation("BigJumpFalling");
		Renderer->CurAnimationPauseOn();
		return;
	}

	if (Duration >= 1.f && Falling == true)
		StateManager.ChangeState("BigJumpEnd");
}

void Monstro::RandomPatern() {
	TurnToPlayer();

	int Rand = rand() % 4;
	if (Rand == 0) {
		StateManager.ChangeState("BigJump");
	} else if (Rand == 1) {
		StateManager.ChangeState("Spit");
	} else {
		StateManager.ChangeState("Jump");
	}
}

void Monstro::TurnToPlayer() {
	float4 PlayerDistance = Player::GetMainPlayer()->GetTransformData().WorldPosition - GetTransformData().WorldPosition;
	float DotProd = float4::DotProduct2D(float4::LEFT, (PlayerDistance.NormalizeReturn()));

	if (DotProd >= 0)
		Direction = 1;
	else 
		Direction = -1;

	TextureSize = Renderer->GetCurTexture()->GetCutScale(0) * Renderer->GetScaleRatio();
	Renderer->GetTransform().SetLocalScale({ Direction * TextureSize.x, TextureSize.y });
}

void Monstro::BigJumpAttack() {
	for (int i = 0; i < 20; i++) {
		Bullet* NewBullet = GetLevel()->CreateActor<Bullet>(Collision->GetOrder());
		NewBullet->Init();
		NewBullet->SetAngle(rand() % 360);
		float BulletSpeed = (rand() % 200) + 200.f;
		NewBullet->SetSpeed(BulletSpeed);
		float4 NewPos = GetTransform().GetWorldPosition();
		NewBullet->GetTransform().SetWorldPosition(NewPos);
		NewBullet->StateManager.ChangeState("Idle");
	}
}
