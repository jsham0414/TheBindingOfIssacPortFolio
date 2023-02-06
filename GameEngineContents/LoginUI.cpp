#include "PreCompile.h"
#include "LoginUI.h"
#include <GameEngineCore/GEngine.h>
#include <GameEngineCore/GameEngineDefaultRenderer.h>

LoginUI::LoginUI()
	: TimeAngle(0.0f)
{
}

LoginUI::~LoginUI()
{
}

GameEngineRenderer* RendererTest = nullptr;

void LoginUI::Start()
{
	// 1280 720
	// GetTransform().SetLocalPosition({ 0, 200, 0 });

	{
		Renderer = CreateComponent<GameEngineTextureRenderer>();
		//Renderer->
		Renderer->SetTexture("Title1.png");
		Renderer->ScaleToTexture();
		Renderer->GetTransform().SetLocalPosition({ 0.0f, 0.f, 800.0f});
		//Renderer->SetPivotToVector({ 0.0f, 0.f });
	}
	GameEngineInput::GetInst()->CreateKey("Space", VK_SPACE);
	GameEngineInput::GetInst()->CreateKey("Esc", VK_ESCAPE);

	StateManager.CreateStateMember("Logo", std::bind(&LoginUI::LogoUpdate, this, std::placeholders::_1, std::placeholders::_2), std::bind(&LoginUI::LogoStart, this, std::placeholders::_1));
	StateManager.CreateStateMember("Select", std::bind(&LoginUI::SelectUpdate, this, std::placeholders::_1, std::placeholders::_2), std::bind(&LoginUI::SelectStart, this, std::placeholders::_1));

	StateManager.ChangeState("Logo");
}

void LoginUI::Update(float _DeltaTime)
{
	StateManager.Update(_DeltaTime);
}

void LoginUI::End()
{
	int a = 0;
}

void LoginUI::LogoUpdate(float _DeltaTime, const StateInfo& _Info) {

	if (GameEngineInput::GetInst()->IsDown("Space")) {
		if (GetLevel()->GetMainCameraActor()->IsMoving() == true)
			return;

		float4 _NewPos = { 0.f, ++MenuIndex * -Renderer->GetTransform().GetWorldScale().y / 2.f };
		GetLevel()->GetMainCameraActor()->CameraMove(_NewPos, 0.3f);
		StateManager.ChangeState("Select");
	}
}

void LoginUI::SelectUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (GameEngineInput::GetInst()->IsDown("Esc")) {
		if (GetLevel()->GetMainCameraActor()->IsMoving() == true)
			return;

		float4 _NewPos = { 0.f, --MenuIndex * -Renderer->GetTransform().GetWorldScale().y / 2.f };
		GetLevel()->GetMainCameraActor()->CameraMove(_NewPos, 0.3f);
		StateManager.ChangeState("Logo");
	}
}

void LoginUI::LogoStart(const StateInfo& _Info) {

}

void LoginUI::SelectStart(const StateInfo& _Info) {

}
