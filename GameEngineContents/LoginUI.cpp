#include "PreCompile.h"
#include "LoginUI.h"
#include <GameEngineCore/GEngine.h>
#include <GameEngineCore/GameEngineDefaultRenderer.h>
#include "LoadingUI.h"

LoginUI::LoginUI()
	: TimeAngle(0.0f)
{
	MenuIndex = 0;
	ArrowIndex = 0;
	IsLoading = false;
}

LoginUI::~LoginUI()
{
}

#define SELECT_BUTTON_COUNT 5

GameEngineRenderer* RendererTest = nullptr;

void LoginUI::Start()
{
	// 1280 720
	// GetTransform().SetLocalPosition({ 0, 200, 0 });

	Renderer = CreateComponent<GameEngineTextureRenderer>();
	DrawRenderer = CreateComponent<GameEngineTextureRenderer>();
	LogoRenderer = CreateComponent<GameEngineTextureRenderer>();
	auto _Renderer = CreateComponent<GameEngineTextureRenderer>();
	CreateFrameAnimation();

	// Title
	Renderer->SetTexture("Title1.png");
	Renderer->ScaleToTexture();
	Renderer->GetTransform().SetLocalPosition({ 0.0f, 0.f, 0.0f });
	//Renderer->SetPivotToVector({ 0.0f, 0.f });

	// Select Menu
	_Renderer->SetParent(this);
	_Renderer->SetTexture("Title2.png");
	_Renderer->ScaleToTexture();
	_Renderer->GetTransform().SetLocalPosition({ 0.0f, -_Renderer->GetTransform().GetWorldScale().y, 0.0f });

	DrawRenderer->SetParent(this);
	DrawRenderer->ChangeFrameAnimation("Draw");
	DrawRenderer->GetTransform().SetLocalPosition({ -26.f, -71.f, 0.f });
	DrawRenderer->ScaleToCutTexture();
	
	LogoRenderer->SetParent(this);
	LogoRenderer->SetTexture("Logo.png");
	LogoRenderer->GetTransform().SetLocalPosition({ 0.f, 148.f, 0.f });
	LogoRenderer->ScaleToTexture();

	auto OverlayRenderer = CreateComponent<GameEngineTextureRenderer>();
	OverlayRenderer->ChangeCamera(CAMERAORDER::UICAMERA);
	OverlayRenderer->SetTexture("menuoverlay.png");
	OverlayRenderer->ScaleToTexture();

	auto ShadowRenderer = CreateComponent<GameEngineTextureRenderer>();
	ShadowRenderer->ChangeCamera(CAMERAORDER::UICAMERA);
	ShadowRenderer->SetTexture("menushadow.png");
	ShadowRenderer->GetTransform().SetLocalPosition({ -224.f, -120.f, 0.f });
	ShadowRenderer->ScaleToTexture();

	const std::string FileName[5] = {
		"NEWRUN.PNG",
		"CONTINUE2.PNG",
		"CHALLINGES.PNG",
		"STATS.PNG",
		"OPTIONS.PNG"
	};

	const float4 AccumulatePosition[5] = {
		{0.f, 0.f},
		{0.f, 0.f},
		{12.f, -8.f},
		{15.f, -28.f},
		{22.f, -30.f},
	};

	for (int i = 0; i < _countof(FileName); i++) {
		auto Button = CreateComponent<GameEngineTextureRenderer>();
		Button->SetTexture(FileName[i]);
		Button->ScaleToTexture();
		float4 FixedPosition = { 6.f, -Renderer->GetTransform().GetWorldScale().y + 144.f };
		Button->GetTransform().SetLocalPosition(FixedPosition + AccumulatePosition[i] + float4(0.f, -60.f * i));
		SelectMenuButtons.push_back(Button);
	}

	SelectArrow = CreateComponent<GameEngineTextureRenderer>();
	SelectArrow->SetTexture("Arrow.png");
	SelectArrow->ScaleToTexture();
	SetArrowPosition();

	KeyBinding();

	StateManager.CreateStateMember("Logo", std::bind(&LoginUI::LogoUpdate, this, std::placeholders::_1, std::placeholders::_2), std::bind(&LoginUI::LogoStart, this, std::placeholders::_1));
	StateManager.CreateStateMember("Select", std::bind(&LoginUI::SelectUpdate, this, std::placeholders::_1, std::placeholders::_2), std::bind(&LoginUI::SelectStart, this, std::placeholders::_1));

	StateManager.ChangeState("Logo");
}

void LoginUI::Update(float _DeltaTime) {
	if (IsLoading)
		return;
	StateManager.Update(_DeltaTime);
}

void LoginUI::End() {
}

void LoginUI::CreateFrameAnimation() {
	DrawRenderer->CreateFrameAnimationCutTexture("Draw", FrameAnimation_DESC("Draw1.png", 0, 1, 0.2f));
}

void LoginUI::KeyBinding() {
	GameEngineInput::GetInst()->CreateKey("Space", VK_SPACE);
	GameEngineInput::GetInst()->CreateKey("Esc", VK_ESCAPE);
	GameEngineInput::GetInst()->CreateKey("Up", VK_UP);
	GameEngineInput::GetInst()->CreateKey("Down", VK_DOWN);
	GameEngineInput::GetInst()->CreateKey("Enter", VK_RETURN);
}

void LoginUI::LogoUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (GameEngineInput::GetInst()->IsDown("Space") ||
		GameEngineInput::GetInst()->IsDown("Enter")) {
		if (GetLevel()->GetMainCameraActor()->IsMoving() == true)
			return;

		MenuIndex++;
		StateManager.ChangeState("Select");
	}
}

void LoginUI::SelectUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (GameEngineInput::GetInst()->IsDown("Esc")) {
		if (GetLevel()->GetMainCameraActor()->IsMoving() == true)
			return;

		MenuIndex--;
		StateManager.ChangeState("Logo");
	}

	if (GameEngineInput::GetInst()->IsDown("Up")) {
		if (--ArrowIndex < 0) ArrowIndex = SELECT_BUTTON_COUNT - 1;
		SetArrowPosition();
	}

	if (GameEngineInput::GetInst()->IsDown("Down")) {
		ArrowIndex = ++ArrowIndex % SELECT_BUTTON_COUNT;
		SetArrowPosition();
	}

	if (GameEngineInput::GetInst()->IsDown("Space") || 
		GameEngineInput::GetInst()->IsDown("Enter")) {
		if (ArrowIndex == 0) {
			IsLoading = true;
			GetLevel()->CreateActor<LoadingUI>();
		}
	}


}

void LoginUI::LogoStart(const StateInfo& _Info) {
	float4 _NewPos = { 0.f, MenuIndex * -Renderer->GetTransform().GetWorldScale().y };
	GetLevel()->GetMainCameraActor()->CameraMove(_NewPos, 1.f);
}

void LoginUI::SelectStart(const StateInfo& _Info) {
	float4 _NewPos = { 0.f, MenuIndex * -Renderer->GetTransform().GetWorldScale().y };
	GetLevel()->GetMainCameraActor()->CameraMove(_NewPos, 1.f);
}

void LoginUI::SetArrowPosition() {
	float4 ArrowPos = SelectMenuButtons[ArrowIndex]->GetTransform().GetLocalPosition();
	ArrowPos.x -= 148.f;
	ArrowPos.y -= 16.f;
	SelectArrow->GetTransform().SetLocalPosition(ArrowPos);
}
