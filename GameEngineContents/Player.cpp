#include "PreCompile.h"
#include "Player.h"

#include <GameEngineContents/GlobalContentsValue.h>
#include <iostream>
#include "ScoreTestComponent.h"
#include "Bullet.h"
#include <GameEngineCore/GameEngineTextureRenderer.cpp>
#include "UIManager.h"
#include "Bomb.h"

Player* Player::MainPlayer = nullptr;

Player::Player()
	: Speed(330.0f) {
	SetName("Player");
	Velocity = { 0.f };
	MainPlayer = this;
	MaxHp = 6;
	Hp = MaxHp;

	for (int i = 0; i < _countof(Stuffs); i++) {
		Stuffs[i] = 0;
	}
}

Player::~Player() {
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
	HeadRenderer->CreateFrameAnimationCutTexture("Idle_Down", FrameAnimation_DESC("issac_head.png", 0, 0, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Idle_Left", FrameAnimation_DESC("issac_head.png", 3, 3, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Idle_Up", FrameAnimation_DESC("issac_head.png", 9, 9, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Idle_Right", FrameAnimation_DESC("issac_head.png", 6, 6, 0.1f));

	HeadRenderer->CreateFrameAnimationCutTexture("Move_Down", FrameAnimation_DESC("issac_head.png", 0, 0, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Move_Left", FrameAnimation_DESC("issac_head.png", 3, 3, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Move_Up", FrameAnimation_DESC("issac_head.png", 9, 9, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Move_Right", FrameAnimation_DESC("issac_head.png", 6, 6, 0.1f));

	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Down", FrameAnimation_DESC("issac_head.png", 0, 2, 0.1f, false));
	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Left", FrameAnimation_DESC("issac_head.png", 3, 5, 0.1f, false));
	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Up", FrameAnimation_DESC("issac_head.png", 9, 11, 0.1f, false));
	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Right", FrameAnimation_DESC("issac_head.png", 6, 8, 0.1f, false));

	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Idle_Down", FrameAnimation_DESC("issac_body_vertical.png", 2, 2, 0.1f, false));

	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Down", FrameAnimation_DESC("issac_body_vertical.png", 0, 9, 0.1f));
	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Up", FrameAnimation_DESC("issac_body_vertical.png", 9, 0, 0.1f));
	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Left", FrameAnimation_DESC("issac_body_horizontal.png", 0, 7, 0.1f));
	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Right", FrameAnimation_DESC("issac_body_horizontal.png", 8, 15, 0.1f));

	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Hit", FrameAnimation_DESC("Issac_hit2.png", 0, 0, 0.3f));
}

void Player::LoseHp(int _Value) {
	Hp -= _Value;
	GetLevel()->GetUIManager()->SetHp(Hp);
}

void Player::Assault(int _Value) {
	if (Invincible)
		return;

	LoseHp(_Value);
	StateManager.ChangeState("Hit");
}

void Player::Start() {
	HeadRenderer = CreateComponent<GameEngineTextureRenderer>();
	HeadRenderer->GetTransform().SetLocalPosition({ 0.0f, 20.f, 10.0f });
	HeadRenderer->SetScaleMode(SCALEMODE::IMAGE);

	BodyRenderer = CreateComponent<GameEngineTextureRenderer>();
	BodyRenderer->GetTransform().SetLocalPosition({ 0.0f, 0.0f, 11.0f });

	KeyBinding();
	CreateFrameAnimation();

	HeadRenderer->ChangeFrameAnimation("Idle_Down");
	BodyRenderer->ChangeFrameAnimation("Issac_Body_Idle_Down");

	HeadRenderer->ScaleToCutTexture();
	BodyRenderer->ScaleToCutTexture();

	Collision = CreateComponent<GameEngineCollision>();
	Collision->GetTransform().SetLocalPosition({ 0, -10, 0 });
	Collision->GetTransform().SetWorldScale({ 30.0f, 30.0f, 50.0f });
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


	// Collision->Collision();

	// 무기명 함수를 만들수 있습니다.
	// 이름없는 인스턴스 함수를 만들수가 있습니다.
	// [](const StateInfo& _Info) 
	//{
	//} 람다를 만들겠다 

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

	int MyValue = 10;

	StateManager.CreateStateMember("Move"
		, std::bind(&Player::MoveUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, nullptr);
	StateManager.ChangeState("Idle");

	GetTransform().Axis = { 0.f };
	GetTransform().Slope = 3.8f;
	Accel = 3.4f;
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
		BodyRenderer->ChangeFrameAnimation("Issac_Body_Idle_Down");
	}
}

void Player::Blinking(DWORD _Duration) {
	DWORD _CurTime = GetTickCount64();
	while (_CurTime + _Duration > GetTickCount64()) {
		Mutex.lock();
		HeadRenderer->GetPixelData().MulColor.a = 0.f;
		BodyRenderer->GetPixelData().MulColor.a = 0.f;
		Mutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		Mutex.lock();
		HeadRenderer->GetPixelData().MulColor.a = 1.f;
		BodyRenderer->GetPixelData().MulColor.a = 1.f;
		Mutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	Mutex.lock();
	Invincible = false;
	Mutex.unlock();
	// TODO : 수평으로 이동하다가 수직 이동 시 가속 붙게 수정
}

void Player::HitStart(const StateInfo& _Info) {
	HeadRenderer->Off();
	BodyRenderer->SetScaleRatio(2.f);
	BodyRenderer->GetTransform().SetLocalPosition({ 0.f, 12.f, 11.f });
	BodyRenderer->ChangeFrameAnimation("Issac_Hit");
	BodyRenderer->ScaleToCutTexture();

	Invincible = true;
	thread(&Player::Blinking, this, 1000).detach();
}

void Player::HitEnd(const StateInfo& _Info) {
	HeadRenderer->On();
	BodyRenderer->SetScaleRatio(2.0f);
	BodyRenderer->GetTransform().SetLocalPosition({ 0.f, 0.f, 11.f });
	BodyRenderer->ChangeFrameAnimation("Issac_Body_Idle_Down");
	BodyRenderer->ScaleToCutTexture();
}

void Player::HitUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (BodyRenderer->CurAnimationEnd()) {
		StateManager.ChangeState("Idle");
	}
}

void Player::HeadAttackStart(const StateInfo& _Info) {
	// 방향키 따라 변경
	if (GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		HeadRenderer->ChangeFrameAnimation("Move_Down");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		HeadRenderer->ChangeFrameAnimation("Move_Up");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		HeadRenderer->ChangeFrameAnimation("Move_Right");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		HeadRenderer->ChangeFrameAnimation("Move_Left");
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
	HeadRenderer->ChangeFrameAnimation("Idle_Down");
}

void Player::HeadAttackUpdate(float _DeltaTime, const StateInfo& _Info) {

	DWORD Delay = 300;
	static DWORD LastTime = GetTickCount64();

	if (!(GameEngineInput::GetInst()->IsPress("AttackLeft") ||
		GameEngineInput::GetInst()->IsPress("AttackRight") ||
		GameEngineInput::GetInst()->IsPress("AttackUp") ||
		GameEngineInput::GetInst()->IsPress("AttackDown")) && HeadRenderer->CurAnimationEnd()) {
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

		LastTime = GetTickCount64() + Delay;
		Bullet* NewBullet = GetLevel()->CreateActor<Bullet>(GetOrder());

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
			_NewPos.x += -10.f + (rand() % 20);

		NewBullet->GetTransform().SetLocalPosition(_NewPos);
		NewBullet->StateManager.ChangeState("Idle");

		switch (HeadDirection) {
		case 2:
			HeadRenderer->ChangeFrameAnimation("Attack_Down", FORCE | RESTART);
			break;
		case 4:
			HeadRenderer->ChangeFrameAnimation("Attack_Left", FORCE | RESTART);
			break;
		case 6:
			HeadRenderer->ChangeFrameAnimation("Attack_Right", FORCE | RESTART);
			break;
		case 8:
			HeadRenderer->ChangeFrameAnimation("Attack_Up", FORCE | RESTART);
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
		HeadRenderer->ChangeFrameAnimation("Move_Down");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		HeadRenderer->ChangeFrameAnimation("Move_Up");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		HeadRenderer->ChangeFrameAnimation("Move_Right");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		HeadRenderer->ChangeFrameAnimation("Move_Left");
	} else {
		HeadRenderer->ChangeFrameAnimation("Idle_Down");
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
		BodyRenderer->ChangeFrameAnimation("Issac_Body_Walk_Down");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		BodyRenderer->ChangeFrameAnimation("Issac_Body_Walk_Up");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		BodyRenderer->ChangeFrameAnimation("Issac_Body_Walk_Right");
	} else if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		BodyRenderer->ChangeFrameAnimation("Issac_Body_Walk_Left");
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

	if (!(GameEngineInput::GetInst()->IsPress("PlayerLeft") || GameEngineInput::GetInst()->IsPress("PlayerRight")))
		GetTransform().Axis.x -= (GetTransform().Axis.x / fabs(GetTransform().Axis.x)) * (GetTransform().Slope) * _DeltaTime;
	if (!(GameEngineInput::GetInst()->IsPress("PlayerUp") || GameEngineInput::GetInst()->IsPress("PlayerDown")))
		GetTransform().Axis.y -= (GetTransform().Axis.y / fabs(GetTransform().Axis.y)) * (GetTransform().Slope) * _DeltaTime;

	GetTransform().Axis.x = fminf(GetTransform().Axis.x, 1.f);
	GetTransform().Axis.y = fminf(GetTransform().Axis.y, 1.f);
	GetTransform().Axis.x = fmaxf(GetTransform().Axis.x, -1.f);
	GetTransform().Axis.y = fmaxf(GetTransform().Axis.y, -1.f);


	float4 _NewForce = { 0.f };

	// 피타고라스
	if (GetHorizontal() != 0.f) {
		float _NewSpeed = GetHorizontal() / sqrtf(fabsf(GetHorizontal()) + fabsf(GetVertical()));
		_NewForce.x = _NewSpeed * Speed;
	}

	if (GetVertical() != 0.f) {
		float _NewSpeed = GetVertical() / sqrtf(fabsf(GetVertical()) + fabsf(GetHorizontal()));
		_NewForce.y = _NewSpeed * Speed;
	}

	float4 asd = (CurrentVelocity * 0.8f + _NewForce * 0.2f);
	GetTransform().SetWorldMove(asd * _DeltaTime);

	CurrentVelocity = _NewForce;

	static DWORD dwTime = GetTickCount64();
	if (dwTime < GetTickCount64()) {
		dwTime = GetTickCount64() + 20;
		//Font->SetText(to_string(asd.x) + ", " + to_string(asd.y));
	}

	if (true == GetLevel()->GetMainCameraActor()->IsFreeCameraMode())
		return;

	float4 qasd = GetTransform().GetWorldPosition();
	qasd.y -= 10.f;
	if (GameEngineInput::GetInst()->IsDown("PlayerBomb"))
		GetLevel()->CreateActor<Bomb>()->GetTransform().SetWorldPosition(qasd);

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
		if (GetTransform().Axis.x > 0.f) GetTransform().Axis.x *= 0.5f;
		GetTransform().Axis.x -= (Accel) * _DeltaTime;
	}
	if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		if (GetTransform().Axis.x < 0.f) GetTransform().Axis.x *= 0.5f;
		GetTransform().Axis.x += (Accel) * _DeltaTime;
	}
	if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		if (GetTransform().Axis.y < 0.f) GetTransform().Axis.y *= 0.5f;
		GetTransform().Axis.y += (Accel) * _DeltaTime;
	}
	if (GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		if (GetTransform().Axis.y > 0.f) GetTransform().Axis.y *= 0.5f;
		GetTransform().Axis.y -= (Accel) * _DeltaTime;
	}

	StateManager.Update(_DeltaTime);
	AttackManager.Update(_DeltaTime);
}