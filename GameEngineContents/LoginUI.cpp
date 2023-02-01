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
		Renderer->GetTransform().SetLocalScale({ 960, 540, 100 });
		Renderer->GetTransform().SetLocalPosition({ 0.0f, 0.0f, 800.0f });
		//Renderer->
		Renderer->SetTexture("Title1.png");
	}
}

void LoginUI::Update(float _DeltaTime)
{
	
}

void LoginUI::End()
{
	int a = 0;
}