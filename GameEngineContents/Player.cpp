#include "PreCompile.h"
#include "Player.h"

#include <GameEngineContents/GlobalContentsValue.h>
#include <iostream>
#include "ScoreTestComponent.h"
#include "Bullet.h"
#include <GameEngineCore/GameEngineTextureRenderer.cpp>
#include "UIManager.h"
#include "Bomb.h"
#include "GameEngineBase/GameEngineThread.h"

Player* Player::MainPlayer = nullptr;

Player::Player()
	: Speed(330.0f) {
	SetName("Player");
	IsRunning = make_shared<atomic<bool>>(true);

	Velocity = { 0.f };
	MainPlayer = this;
	Info.MaxHP = 6;
	Info.HP = Info.MaxHP;
	Info.BaseDamage = 1.f;
	Info.AttackSpeed = 0.f;

	for (int i = 0; i < _countof(Stuffs); i++) {
		Stuffs[i] = 0;
	}
}

Player::~Player() {
	*IsRunning = false;
}

void Player::KeyBinding() {
	if (false == GameEngineInput::GetInst()->IsKey("PlayerLeft")) {
		GameEngineInput::GetInst()->CreateKey("PlayerLeft", 'A');
		GameEngineInput::GetInst()->CreateKey("PlayerRight", 'D');
		GameEngineInput::GetInst()->CreateKey("PlayerUp", 'W');
		GameEngineInput::GetInst()->CreateKey("PlayerDown", 'S');
		GameEngineInput::GetInst()->CreateKey("AttackLeft", VK_LEFT);
		GameEngineInput::GetInst()->CreateKey("AttackRight", VK_RIGHT);
		GameEngineInput::GetInst()->CreateKey("AttackUp", VK_UP);
		GameEngineInput::GetInst()->CreateKey("AttackDown", VK_DOWN);
		GameEngineInput::GetInst()->CreateKey("PlayerF", VK_NUMPAD1);
		GameEngineInput::GetInst()->CreateKey("PlayerB", VK_NUMPAD2);
		GameEngineInput::GetInst()->CreateKey("PlayerBomb", 'E');

	}
}

void Player::CreateFrameAnimation() {
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Idle_Down", FrameAnimation_DESC("issac_head.png", 0, 0, 0.1f));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Idle_Left", FrameAnimation_DESC("issac_head.png", 3, 3, 0.1f));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Idle_Up", FrameAnimation_DESC("issac_head.png", 9, 9, 0.1f));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Idle_Right", FrameAnimation_DESC("issac_head.png", 6, 6, 0.1f));

	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Move_Down", FrameAnimation_DESC("issac_head.png", 0, 0, 0.1f));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Move_Left", FrameAnimation_DESC("issac_head.png", 3, 3, 0.1f));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Move_Up", FrameAnimation_DESC("issac_head.png", 9, 9, 0.1f));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Move_Right", FrameAnimation_DESC("issac_head.png", 6, 6, 0.1f));

	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Attack_Down", FrameAnimation_DESC("issac_head.png", 0, 2, 0.1f, false));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Attack_Left", FrameAnimation_DESC("issac_head.png", 3, 5, 0.1f, false));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Attack_Up", FrameAnimation_DESC("issac_head.png", 9, 11, 0.1f, false));
	AllRenderer["Head"]->CreateFrameAnimationCutTexture("Attack_Right", FrameAnimation_DESC("issac_head.png", 6, 8, 0.1f, false));

	AllRenderer["Body"]->CreateFrameAnimationCutTexture("Issac_Body_Idle_Down", FrameAnimation_DESC("issac_body_vertical.png", 2, 2, 0.1f, false));

	AllRenderer["Body"]->CreateFrameAnimationCutTexture("Issac_Body_Walk_Down", FrameAnimation_DESC("issac_body_vertical.png", 0, 9, 0.1f));
	AllRenderer["Body"]->CreateFrameAnimationCutTexture("Issac_Body_Walk_Up", FrameAnimation_DESC("issac_body_vertical.png", 9, 0, 0.1f));
	AllRenderer["Body"]->CreateFrameAnimationCutTexture("Issac_Body_Walk_Left", FrameAnimation_DESC("issac_body_horizontal.png", 0, 7, 0.1f));
	AllRenderer["Body"]->CreateFrameAnimationCutTexture("Issac_Body_Walk_Right", FrameAnimation_DESC("issac_body_horizontal.png", 8, 15, 0.1f));

	AllRenderer["Body"]->CreateFrameAnimationCutTexture("Issac_Hit", FrameAnimation_DESC("Issac_hit2.png", 0, 0, 0.3f));
	AllRenderer["Body"]->CreateFrameAnimationCutTexture("Issac_Loot", FrameAnimation_DESC("Issac_Loot.png", 0, 0, 1.f));

}

void Player::LoseHp(int _Value) {
	Info.HP -= _Value;
	GetLevel()->GetUIManager()->SetHp(Info.HP);
}

void Player::Assault(int _Value) {
	if (Invincible)
		return;

	LoseHp(_Value);
	StateManager.ChangeState("Hit");
}

void Player::AddStuff(int _Type, int _Value) {
	Stuffs[_Type] += _Value;
	GetLevel()->GetUIManager()->SetStuff(_Type, _Value);
}

void Player::LootItem() {
	if (Invincible)
		return;

	StateManager.ChangeState("Loot");
}

void Player::Start() {
	AllRenderer["Head"] = CreateComponent<GameEngineTextureRenderer>();
	AllRenderer["Head"]->GetTransform().SetLocalPosition({ 0.0f, 20.f, 10.0f });

	AllRenderer["Body"] = CreateComponent<GameEngineTextureRenderer>();
	AllRenderer["Body"]->GetTransform().SetLocalPosition({ 0.0f, 0.0f, 11.0f });

	auto Shadow = CreateComponent<GameEngineTextureRenderer>();
	Shadow->GetTransform().SetLocalPosition({ 0.0f, -14.0f, 11.0f });
	Shadow->SetTexture("shadow.png");
	Shadow->GetPixelData().MulColor.a = 0.2f;
	Shadow->SetScaleRatio(0.35f);
	Shadow->ScaleToTexture();

	KeyBinding();
	CreateFrameAnimation();

	AllRenderer["Head"]->ChangeFrameAnimation("Idle_Down");
	AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Idle_Down");

	AllRenderer["Head"]->ScaleToCutTexture();
	AllRenderer["Body"]->ScaleToCutTexture();

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalPosition({ 0, -10, 0 });
	Collision->GetTransform().SetWorldScale({ 30.0f, 30.0f });
	Collision->ChangeOrder(OBJECTORDER::Player);
	float4 Color = float4::RED;
	Color.a = 0.2f;
	Collision->SetDebugSetting(CollisionType::CT_AABB, Color);

	Font = CreateComponent<GameEngineFontRenderer>();
	Font->SetRenderingOrder(10000);
	Font->SetText("");
	Font->SetColor({ 1.0f, 1.0f, 1.0f, 1.0 });
	Font->SetSize(20);
	Font->SetScreenPostion({ 0, 0 });
	Font->ChangeCamera(CAMERAORDER::UICAMERA);

	AttackManager.CreateStateMember("Idle"
		, std::bind(&Player::HeadIdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Player::HeadIdleStart, this, std::placeholders::_1)
	);

	AttackManager.CreateStateMember("Attack"
		, std::bind(&Player::HeadAttackUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Player::HeadAttackStart, this, std::placeholders::_1)
	);
	AttackManager.ChangeState("Idle");

	StateManager.CreateStateMember("Idle"
		, std::bind(&Player::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Player::IdleStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Hit",
		std::bind(&Player::HitUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Player::HitStart, this, std::placeholders::_1),
		std::bind(&Player::HitEnd, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Loot",
		std::bind(&Player::LootUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Player::LootStart, this, std::placeholders::_1),
		std::bind(&Player::LootEnd, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Move"
		, std::bind(&Player::MoveUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, nullptr);
	StateManager.ChangeState("Idle");

	GetTransform().Axis = { 0.f };
	GetTransform().Slope = 3.f;
	Accel = 3.8f;
}

void Player::IdleStart(const StateInfo& _Info) {

}

float Player::GetVertical() {
	return trunc(GetTransform().Axis.y * 100.f) / 100.f;
}

float Player::GetHorizontal() {
	return trunc(GetTransform().Axis.x * 100.f) / 100.f;
}

void Player::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	// HPRenderer->GetTransform().AddLocalScale(float4(-5.0f * _DeltaTime, 0.0f, 0.0f, 0.0f));

	if (true == GameEngineInput::GetInst()->IsPress("PlayerLeft") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerRight") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerUp") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		//Renderer->CurAnimationReset();
		StateManager.ChangeState("Move");
		return;
	}

	if (GetVertical() == 0.f && GetHorizontal() == 0.f) {
		AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Idle_Down");
	}
}

void Player::BlinkingThread(ULONGLONG _Duration) {
	float Alpha = 0.f;

	ULONGLONG _CurTime = GetTickCount64();
	while (_CurTime + _Duration > GetTickCount64()) {
		Mutex.lock();
		AllRenderer["Head"]->GetPixelData().MulColor.a = Alpha;
		AllRenderer["Body"]->GetPixelData().MulColor.a = Alpha;
		Mutex.unlock();

		Alpha = (float)!Alpha;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	Mutex.lock();
	AllRenderer["Head"]->GetPixelData().MulColor.a = 1.f;
	AllRenderer["Body"]->GetPixelData().MulColor.a = 1.f;
	Invincible = false;
	Mutex.unlock();
}

void Player::LootStart(const StateInfo& _Info) {
	AllRenderer["Head"]->Off();
	AllRenderer["Body"]->SetScaleRatio(2.f);
	AllRenderer["Body"]->GetTransform().SetLocalPosition({ 0.f, 12.f, 10.f });
	AllRenderer["Body"]->ChangeFrameAnimation("Issac_Loot");
	AllRenderer["Body"]->ScaleToCutTexture();

	Invincible = true;
}

void Player::LootEnd(const StateInfo& _Info) {
	AllRenderer["Head"]->On();
	AllRenderer["Body"]->SetScaleRatio(2.0f);
	AllRenderer["Body"]->GetTransform().SetLocalPosition({ 0.f, 0.f, 10.f });
	AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Idle_Down");
	AllRenderer["Body"]->ScaleToCutTexture();
	Invincible = false;
}

void Player::LootUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (AllRenderer["Body"]->CurAnimationEnd()) {
		StateManager.ChangeState("Idle");
	}
}

void Player::HitStart(const StateInfo& _Info) {
	AllRenderer["Head"]->Off();
	AllRenderer["Body"]->SetScaleRatio(2.f);
	AllRenderer["Body"]->GetTransform().SetLocalPosition({ 0.f, 12.f, 10.f });
	AllRenderer["Body"]->ChangeFrameAnimation("Issac_Hit");
	AllRenderer["Body"]->ScaleToCutTexture();

	Invincible = true;

	auto t = GameEngineThread(&Player::BlinkingThread, this, 1000);
	t.detach();
	
}

void Player::HitEnd(const StateInfo& _Info) {
	AllRenderer["Head"]->On();
	AllRenderer["Body"]->SetScaleRatio(2.0f);
	AllRenderer["Body"]->GetTransform().SetLocalPosition({ 0.f, 0.f, 10.f });
	AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Idle_Down");
	AllRenderer["Body"]->ScaleToCutTexture();
}

void Player::HitUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (AllRenderer["Body"]->CurAnimationEnd()) {
		StateManager.ChangeState("Idle");
	}
}

void Player::HeadAttackStart(const StateInfo& _Info) {
	// 방향키 따라 변경
	if (GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Down");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Up");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Right");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Left");
	}
}

void Player::HeadIdleStart(const StateInfo& _Info) {
	if (true == GameEngineInput::GetInst()->IsPress("PlayerLeft") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerRight") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerUp") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		//Renderer->CurAnimationReset();
		return;
	}
	AllRenderer["Head"]->ChangeFrameAnimation("Idle_Down");
}

void Player::HeadAttackUpdate(float _DeltaTime, const StateInfo& _Info) {

	DWORD Delay = 300;
	static DWORD LastTime = GetTickCount64();

	if (!(GameEngineInput::GetInst()->IsPress("AttackLeft") ||
		GameEngineInput::GetInst()->IsPress("AttackRight") ||
		GameEngineInput::GetInst()->IsPress("AttackUp") ||
		GameEngineInput::GetInst()->IsPress("AttackDown")) && AllRenderer["Head"]->CurAnimationEnd()) {
		AttackManager.ChangeState("Idle");
		return;
	}

	if (GameEngineInput::GetInst()->IsPress("AttackLeft") ||
		GameEngineInput::GetInst()->IsPress("AttackRight") ||
		GameEngineInput::GetInst()->IsPress("AttackUp") ||
		GameEngineInput::GetInst()->IsPress("AttackDown")) {
		if (LastTime > GetTickCount64()) {
			return;
		}

		float asd = Info.GetAttackSpeed() * 1000.f;
		LastTime = GetTickCount64() + asd;
		Bullet* NewBullet = GetLevel()->CreateActor<Bullet>(GetOrder());
		NewBullet->Init();

		float NewAngle = 0;
		float _PlusAngle = 10.f; // 1당 휘어질 각도

		float4 _TempAxis = GetTransform().Axis;
		switch (HeadDirection) {
		case 2:
			NewAngle = 270;
			NewAngle += _PlusAngle * GetTransform().Axis.x;
			if (GetTransform().Axis.y < 0) NewBullet->AddSpeed(Speed * 0.8f * fabsf(_TempAxis.y));
			break;
		case 4:
			NewAngle = 180;
			NewAngle += _PlusAngle * -GetTransform().Axis.y;
			if (GetTransform().Axis.x < 0) NewBullet->AddSpeed(Speed * 0.8f * fabsf(_TempAxis.x));
			break;
		case 6:
			NewAngle = 0;
			NewAngle += _PlusAngle * GetTransform().Axis.y;
			if (GetTransform().Axis.x > 0) NewBullet->AddSpeed(Speed * 0.8f * fabsf(_TempAxis.x));
			break;
		case 8:
			NewAngle = 90;
			NewAngle += _PlusAngle * -GetTransform().Axis.x;
			if (GetTransform().Axis.y > 0)NewBullet->AddSpeed(Speed * 0.8f * fabsf(_TempAxis.y));
			break;
		}

		NewBullet->SetAngle(NewAngle);
		float4 _NewPos = GetTransform().GetWorldPosition();
		_NewPos.y += 15;

		if (HeadDirection == 4 || HeadDirection == 6)
			_NewPos.y += (rand() % 20);
		if (HeadDirection == 2 || HeadDirection == 8)
			_NewPos.x += -15.f + (rand() % 30);

		NewBullet->GetTransform().SetLocalPosition(_NewPos);
		NewBullet->StateManager.ChangeState("Idle");

		switch (HeadDirection) {
		case 2:
			AllRenderer["Head"]->ChangeFrameAnimation("Attack_Down", FORCE | RESTART);
			break;
		case 4:
			AllRenderer["Head"]->ChangeFrameAnimation("Attack_Left", FORCE | RESTART);
			break;
		case 6:
			AllRenderer["Head"]->ChangeFrameAnimation("Attack_Right", FORCE | RESTART);
			break;
		case 8:
			AllRenderer["Head"]->ChangeFrameAnimation("Attack_Up", FORCE | RESTART);
			break;
		}
		return;
	}


}

void Player::HeadIdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (GameEngineInput::GetInst()->IsPress("AttackLeft") ||
		GameEngineInput::GetInst()->IsPress("AttackRight") ||
		GameEngineInput::GetInst()->IsPress("AttackUp") ||
		GameEngineInput::GetInst()->IsPress("AttackDown")) {
		AttackManager.ChangeState("Attack");
		return;
	}

	if (GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Down");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Up");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Right");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		AllRenderer["Head"]->ChangeFrameAnimation("Move_Left");
	} else {
		AllRenderer["Head"]->ChangeFrameAnimation("Idle_Down");
	}

}

//void Player::MoveStart(const StateInfo& _Info) 
//{
//	Renderer->ChangeFrameAnimation("Move");
//}

void Player::MoveUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (false == GameEngineInput::GetInst()->IsPress("PlayerLeft") &&
		false == GameEngineInput::GetInst()->IsPress("PlayerRight") &&
		false == GameEngineInput::GetInst()->IsPress("PlayerUp") &&
		false == GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		StateManager.ChangeState("Idle");
		return;
	}

	if (GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Walk_Down");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Walk_Up");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Walk_Right");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		AllRenderer["Body"]->ChangeFrameAnimation("Issac_Body_Walk_Left");
	}
}

bool Player::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	StateManager.ChangeState("Hit");
	LoseHp();
	return true;
}



void Player::Attack(int _Direction) {

}

void Player::PlayerSetPosition(float4 _NewPos) {
	GetTransform().SetWorldPosition(_NewPos);
}

void Player::Update(float _DeltaTime) {
	static float4 CurrentVelocity = { 0.f };

	if (!(GameEngineInput::GetInst()->IsPress("PlayerLeft") || GameEngineInput::GetInst()->IsPress("PlayerRight")) && (GetTransform().Axis.x != 0.f)) {
		Decel = (GetTransform().Axis.x / fabs(GetTransform().Axis.x)) * (GetTransform().Slope) * _DeltaTime;
		if ((GetTransform().Axis.x > 0.f && GetTransform().Axis.x - Decel > 0.f) || (GetTransform().Axis.x < 0.f && GetTransform().Axis.x - Decel < 0.f)) {
			GetTransform().Axis.x -= Decel;
		} else if ((GetTransform().Axis.x > 0.f && GetTransform().Axis.x - Decel <= 0.f) || (GetTransform().Axis.x < 0.f && GetTransform().Axis.x - Decel >= 0.f)) {
			GetTransform().Axis.x = 0.f;
		}
	}

	if (!(GameEngineInput::GetInst()->IsPress("PlayerUp") || GameEngineInput::GetInst()->IsPress("PlayerDown")) && (GetTransform().Axis.y != 0.f)) {
		Decel = (GetTransform().Axis.y / fabs(GetTransform().Axis.y)) * (GetTransform().Slope) * _DeltaTime;
		if ((GetTransform().Axis.y > 0.f && GetTransform().Axis.y - Decel > 0.f) || (GetTransform().Axis.y < 0.f && GetTransform().Axis.y - Decel < 0.f)) {
			GetTransform().Axis.y -= Decel;
		} else if ((GetTransform().Axis.y > 0.f && GetTransform().Axis.y - Decel <= 0.f) || (GetTransform().Axis.y < 0.f && GetTransform().Axis.y - Decel >= 0.f)) {
			GetTransform().Axis.y = 0.f;
		}
	}
	GetTransform().Axis.x = fminf(GetTransform().Axis.x, 1.f);
	GetTransform().Axis.y = fminf(GetTransform().Axis.y, 1.f);
	GetTransform().Axis.x = fmaxf(GetTransform().Axis.x, -1.f);
	GetTransform().Axis.y = fmaxf(GetTransform().Axis.y, -1.f);

	Collision->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Monster, CollisionType::CT_OBB2D);

	float4 _NewForce = { 0.f };

	if (GetHorizontal() != 0.f) {
		float _NewSpeed = GetHorizontal() / sqrtf(fabsf(GetHorizontal()) + fabsf(GetVertical()));
		_NewForce.x = _NewSpeed * Speed;
	}

	if (GetVertical() != 0.f) {
		float _NewSpeed = GetVertical() / sqrtf(fabsf(GetVertical()) + fabsf(GetHorizontal()));
		_NewForce.y = _NewSpeed * Speed;
	}
	
	float4 qwe = GetTransform().Axis;
	float4 SmoothMove = (CurrentVelocity * 0.8f + _NewForce * 0.2f);
	GetTransform().SetWorldMove(SmoothMove * _DeltaTime);

	CurrentVelocity = _NewForce;

	static DWORD dwTime = GetTickCount64();
	if (dwTime < GetTickCount64()) {
		dwTime = GetTickCount64() + 20;
		//Font->SetText(to_string(asd.x) + ", " + to_string(asd.y));
	}

	if (true == GetLevel()->GetMainCameraActor()->IsFreeCameraMode())
		return;

	if (GameEngineInput::GetInst()->IsDown("PlayerBomb")) {
		Bomb* NewBomb = GetLevel()->CreateActor<Bomb>();
		float4 Fixed = GetTransform().GetWorldPosition();
		Fixed.y -= 16.f;
		Fixed.z += 100.f;
		NewBomb->GetTransform().SetWorldPosition(Fixed);
	}

	if (GameEngineInput::GetInst()->IsPress("AttackUp")) {
		HeadDirection = 8;
	} else if (GameEngineInput::GetInst()->IsPress("AttackDown")) {
		HeadDirection = 2;
	} else if (GameEngineInput::GetInst()->IsPress("AttackLeft")) {
		HeadDirection = 4;
	} else if (GameEngineInput::GetInst()->IsPress("AttackRight")) {
		HeadDirection = 6;
	}

	if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		if (GetTransform().Axis.x == 0.f && GetTransform().Axis.y != 0.f) GetTransform().Axis.x = GetTransform().Axis.y * 0.25f;
		if (GetTransform().Axis.x > 0.f) GetTransform().Axis.x *= 0.75f;

		GetTransform().Axis.x -= (Accel) * _DeltaTime;
	}
	if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		if (GetTransform().Axis.x == 0.f && GetTransform().Axis.y != 0.f) GetTransform().Axis.x = GetTransform().Axis.y * 0.25f;
		if (GetTransform().Axis.x < 0.f) GetTransform().Axis.x *= 0.75f;

		GetTransform().Axis.x += (Accel) * _DeltaTime;
	}
	if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		if (GetTransform().Axis.y == 0.f && GetTransform().Axis.x != 0.f) GetTransform().Axis.y = GetTransform().Axis.x * 0.25f;
		if (GetTransform().Axis.y < 0.f) GetTransform().Axis.y *= 0.75f;

		GetTransform().Axis.y += (Accel) * _DeltaTime;
	}
	if (GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		if (GetTransform().Axis.y == 0.f && GetTransform().Axis.x != 0.f) GetTransform().Axis.y = GetTransform().Axis.x * 0.25f;
		if (GetTransform().Axis.y > 0.f) GetTransform().Axis.y *= 0.75f;

		GetTransform().Axis.y -= (Accel) * _DeltaTime;
	}

	StateManager.Update(_DeltaTime);
	AttackManager.Update(_DeltaTime);
}
