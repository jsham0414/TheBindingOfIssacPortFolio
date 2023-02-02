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
		GameEngineTextureRenderer* Renderer = CreateComponent<GameEngineTextureRenderer>();
		//Renderer->
		Renderer->SetTexture("Title1.png");
		Renderer->ScaleToTexture();
		Renderer->GetTransform().SetLocalPosition({ 0.0f, -Renderer->GetTransformData().WorldScaling.y / 4.f, 800.0f});
		//Renderer->SetPivotToVector({ 0.0f, 0.f });
	}
	GameEngineInput::GetInst()->CreateKey("Space", VK_SPACE);
}

void LoginUI::Update(float _DeltaTime)
{
	if (GameEngineInput::GetInst()->IsDown("Space")) {

	}
}

void LoginUI::End()
{
	int a = 0;
}