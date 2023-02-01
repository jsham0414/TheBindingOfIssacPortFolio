#include "PreCompile.h"
#include "Player.h"

#include <GameEngineContents/GlobalContentsValue.h>
#include <iostream>
#include "ScoreTestComponent.h"
#include "Bullet.h"
#include <GameEngineCore/GameEngineTextureRenderer.cpp>
#include "UIManager.h"

Player* Player::MainPlayer = nullptr;

Player::Player()
	: Speed(250.0f) {
	SetName("Player");
	PlayerHorizontal = 0.f;
	PlayerVertical = 0.f;
	Slope = 0.015f;
	Accel = 0.03f;
	MainPlayer = this;
	MaxHp = 6;
	Hp = MaxHp;

	for (int i = 0; i < _countof(Stuffs); i++) {
		Stuffs[i] = 0;
	}
}

Player::~Player()
{
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
	}
}

void Player::CreateFrameAnimation() {
	HeadRenderer->CreateFrameAnimationCutTexture("Idle_Down", FrameAnimation_DESC("issac_head.png", 0, 0, 0.3f));

	HeadRenderer->CreateFrameAnimationCutTexture("Move_Down", FrameAnimation_DESC("issac_head.png", 0, 0, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Move_Right", FrameAnimation_DESC("issac_head.png", 3, 3, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Move_Up", FrameAnimation_DESC("issac_head.png", 6, 6, 0.1f));
	HeadRenderer->CreateFrameAnimationCutTexture("Move_Left", FrameAnimation_DESC("issac_head.png", 9, 9, 0.1f));

	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Down", FrameAnimation_DESC("issac_head.png", 0, 2, 0.1f, false));
	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Right", FrameAnimation_DESC("issac_head.png", 3, 5, 0.1f, false));
	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Up", FrameAnimation_DESC("issac_head.png", 6, 8, 0.1f, false));
	HeadRenderer->CreateFrameAnimationCutTexture("Attack_Left", FrameAnimation_DESC("issac_head.png", 9, 11, 0.1f, false));

	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Idle_Down", FrameAnimation_DESC("issac_body_vertical.png", 0, 0, 0.3f));

	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Down", FrameAnimation_DESC("issac_body_vertical.png", 0, 4, 0.1f));
	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Up", FrameAnimation_DESC("issac_body_vertical.png", 5, 9, 0.1f));
	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Left", FrameAnimation_DESC("issac_body_horizontal.png", 0, 4, 0.1f));
	BodyRenderer->CreateFrameAnimationCutTexture("Issac_Body_Walk_Right", FrameAnimation_DESC("issac_body_horizontal.png", 5, 9, 0.1f));
}

void Player::Start() {
	HeadRenderer = CreateComponent<GameEngineTextureRenderer>();
	HeadRenderer->GetTransform().SetWorldScale({ 40 * 1.5f, 32 * 1.5f, 1 });
	HeadRenderer->GetTransform().SetLocalPosition({ 0.0f, 9.f, 10.0f });

	BodyRenderer = CreateComponent<GameEngineTextureRenderer>();
	BodyRenderer->GetTransform().SetWorldScale({ 40 * 1.5f, 32 * 1.5f, 1 });
	BodyRenderer->GetTransform().SetLocalPosition({ 0.0f, 0.0f, 11.0f });

	KeyBinding();
	CreateFrameAnimation();

	HeadRenderer->ChangeFrameAnimation("Idle_Down");
	BodyRenderer->ChangeFrameAnimation("Issac_Body_Idle_Down");

	//GetTransform().SetLocalScale({ 1, 1, 1 });


	//ScoreTestComponent* ScoreCom = CreateComponent<ScoreTestComponent>();
	//{
	//	{
	//		HPRenderer = CreateComponent<GameEngineTextureRenderer>();
	//		// HPRenderer->GetTransform().SetLocalPosition({ 200, 150, 20 });
	//		HPRenderer->GetTransform().SetWorldScale({ 300, 300, 1 });
	//		HPRenderer->SetFolderTextureToIndex("Play", 0);
	//		HPRenderer->ScaleToTexture();

	//		// HPRenderer->SetTexture(nullptr);

	//		// HPRenderer->SetPivot(PIVOTMODE::CUSTOM);

	//	}


	//}

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
	Font->SetColor( { 1.0f, 1.0f, 1.0f, 1.0 });
	Font->SetSize(20);
	Font->SetScreenPostion({0, 0});
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

	int MyValue = 10;

	StateManager.CreateStateMember("Move"
		, std::bind(&Player::MoveUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, [/*&*/=](const StateInfo& _Info)
		{
			// static const int MyValue = 바깥 MyValue;

			int Test = MyValue;
			// = 지역변수도 쓸수있다.
			// MyValue가 하나더 생기는 방식으로 컴파일러가 해석한다.
			// ????????
			// & 외부의 있는 값의 참조형을 받아오는 것이기 때문에
			// 지역변수를 쓰면 결과를 장담할수가 없다.
			//Renderer->ChangeFrameAnimation("Move");

		});
	StateManager.ChangeState("Idle");
	

	// GetTransform().SetWorldPosition({ 100.0f, 0.0f, 100.0f });
	// GetTransform().SetWorldPosition({ 300.0f, 200.0f, 100.0f });
	// GetTransform().SetWorldPosition({ 0.0f, 0.0f, 100.0f });
}

void Player::IdleStart(const StateInfo& _Info) {
	HeadRenderer->ChangeFrameAnimation("Idle_Down");
}

float Player::GetVertical() {
	return trunc(PlayerVertical * 100.f) / 100.f;
}

float Player::GetHorizontal() {
	return trunc(PlayerHorizontal * 100.f) / 100.f;
}

void Player::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	// HPRenderer->GetTransform().AddLocalScale(float4(-5.0f * _DeltaTime, 0.0f, 0.0f, 0.0f));
	if (GetVertical() == 0.f && GetHorizontal() == 0.f) {
		BodyRenderer->ChangeFrameAnimation("Issac_Body_Idle_Down");
	}

	if (true == GameEngineInput::GetInst()->IsPress("PlayerLeft") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerRight") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerUp") ||
		true == GameEngineInput::GetInst()->IsPress("PlayerDown"))
	{
		//Renderer->CurAnimationReset();
		StateManager.ChangeState("Move");
	}
}

void Player::HeadAttackStart(const StateInfo& _Info) {

	// 방향키 따라 변경

}

void Player::HeadIdleStart(const StateInfo& _Info) {
	HeadRenderer->ChangeFrameAnimation("Idle_Down");
}

void Player::HeadAttackUpdate(float _DeltaTime, const StateInfo& _Info) {

	DWORD Delay = 500;
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


		Hp--;
		if (Hp < 0) Hp = MaxHp;
		GetLevel()->GetUIManager()->SetHp(Hp);

		LastTime = GetTickCount64() + Delay;
		Bullet* NewBullet = GetLevel()->CreateActor<Bullet>(GetOrder());
		NewBullet->SetDirection(HeadDirection);
		NewBullet->GetTransform().SetLocalPosition(GetTransform().GetLocalPosition());
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
	}

}

//void Player::MoveStart(const StateInfo& _Info) 
//{
//	Renderer->ChangeFrameAnimation("Move");
//}

void Player::MoveUpdate(float _DeltaTime, const StateInfo& _Info)
{
	//if (_Info.StateTime >= 2.0f)
	//{
	//	StateManager.ChangeState("Idle");
	//	return;
	//}

	// GameEngineDebug::DebugSphereRender();
	
	if (false == GameEngineInput::GetInst()->IsPress("PlayerLeft") &&
		false == GameEngineInput::GetInst()->IsPress("PlayerRight") &&
		false == GameEngineInput::GetInst()->IsPress("PlayerUp") &&
		false == GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		StateManager.ChangeState("Idle");
		return;
	}

	//ImGui::Text(to_string(PlayerHorizontal).c_str());
	//ImGui::Text(to_string(PlayerVertical).c_str());

	if (GameEngineInput::GetInst()->IsPress("PlayerLeft")) {
		PlayerHorizontal += -(Speed * Accel) * _DeltaTime;
	} 
	if (GameEngineInput::GetInst()->IsPress("PlayerRight")) {
		PlayerHorizontal += (Speed * Accel) * _DeltaTime;
	} 
	if (GameEngineInput::GetInst()->IsPress("PlayerUp")) {
		PlayerVertical += (Speed * Accel) * _DeltaTime;
	} 
	if (GameEngineInput::GetInst()->IsPress("PlayerDown")) {
		PlayerVertical += -(Speed * Accel) * _DeltaTime;
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

	//PlayerHorizontal, PlayerVertical;
	//StateManager.ChangeState("Move");

	//if (true == GameEngineInput::GetInst()->IsPress("PlayerLeft"))
	//{
	//	GetTransform().SetWorldMove(GetTransform().GetLeftVector() * (Speed * PlayerHorizontal) * _DeltaTime);
	//	//Renderer->GetTransform().PixLocalNegativeX();
	//}

	//if (true == GameEngineInput::GetInst()->IsPress("PlayerRight"))
	//{
	//	GetTransform().SetWorldMove(GetTransform().GetRightVector() * (Speed * PlayerHorizontal) * _DeltaTime);
	//	//Renderer->GetTransform().PixLocalPositiveX();
	//	// Renderer->GetColorData().MulColor.a -= _DeltaTime;
	//}
	//if (true == GameEngineInput::GetInst()->IsPress("PlayerUp"))
	//{
	//	GetTransform().SetWorldMove(GetTransform().GetUpVector() * (Speed * PlayerVertical) * _DeltaTime);
	//}
	//if (true == GameEngineInput::GetInst()->IsPress("PlayerDown"))
	//{
	//	GetTransform().SetWorldMove(GetTransform().GetDownVector() * (Speed * PlayerVertical) * _DeltaTime);
	//}


	//if (true == GameEngineInput::GetInst()->IsPress("PlayerF"))
	//{
	//	GetTransform().SetWorldMove(GetTransform().GetForwardVector() * Speed * _DeltaTime);
	//}
	//if (true == GameEngineInput::GetInst()->IsPress("PlayerB"))
	//{
	//	GetTransform().SetWorldMove(GetTransform().GetBackVector() * Speed * _DeltaTime);
	//}

}

bool Player::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other)
{
	//_Other->GetActor()->Death();

	return true;
}



void Player::Attack(int _Direction) {

}

void Player::Update(float _DeltaTime) {
	for (int i = 0; i < _countof(Stuffs); i++) {
		Stuffs[i] -= 1;

		if (Stuffs[i] < 0) {
			Stuffs[i] = 99;
		}
		GetLevel()->GetUIManager()->SetStuff(i, Stuffs[i]);
	}

	if (!(GameEngineInput::GetInst()->IsPress("PlayerUp") || GameEngineInput::GetInst()->IsPress("PlayerDown")))
		PlayerVertical -= (PlayerVertical / fabs(PlayerVertical)) * (Speed * Slope) * _DeltaTime;
	if (!(GameEngineInput::GetInst()->IsPress("PlayerLeft") || GameEngineInput::GetInst()->IsPress("PlayerRight")))
		PlayerHorizontal -= (PlayerHorizontal / fabs(PlayerHorizontal)) * (Speed * Slope) * _DeltaTime;

	PlayerVertical = fminf(PlayerVertical, 1.f);
	PlayerHorizontal = fminf(PlayerHorizontal, 1.f);
	PlayerVertical = fmaxf(PlayerVertical, -1.f);
	PlayerHorizontal = fmaxf(PlayerHorizontal, -1.f);

	static DWORD dwTime = GetTickCount64();
	if (dwTime < GetTickCount64()) {
		dwTime = GetTickCount64() + 10;
		//Font->SetText(to_string(GetVertical()) + ", " + to_string(GetHorizontal()));
	}

	GetTransform().SetWorldMove(GetTransform().GetUpVector() * (Speed * sin(GetVertical())) * _DeltaTime);
	GetTransform().SetWorldMove(GetTransform().GetRightVector() * (Speed * sin(GetHorizontal())) * _DeltaTime);

	if (true == GetLevel()->GetMainCameraActor()->IsFreeCameraMode()) {
		return;
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

	// StateManager 기능으로 
	StateManager.Update(_DeltaTime);
	AttackManager.Update(_DeltaTime);

	Collision->IsCollision(CollisionType::CT_AABB, OBJECTORDER::UI, CollisionType::CT_AABB,
		std::bind(&Player::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2)
	);
}