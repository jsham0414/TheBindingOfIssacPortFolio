#include "PreCompile.h"
#include "DyingBlood.h"

DyingBlood::DyingBlood() {
	PrevTime = 0.f;
}

DyingBlood::~DyingBlood() {
}

void DyingBlood::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Effect = CreateComponent<GameEngineTextureRenderer>();
	CreateFrameAnimation();

	Renderer->GetTransform().SetLocalPosition({ 0.f, 0.f, 4001.f });

	Effect->GetTransform().SetLocalPosition({ 0.f, 0.f, 0.f });
	Effect->ChangeFrameAnimation("Blood");
	Effect->ScaleToCutTexture();
}

void DyingBlood::CreateFrameAnimation() {
	Effect->CreateFrameAnimationCutTexture(
		"Blood",
		FrameAnimation_DESC("effect_002_bloodpoof.png", 0, 9, 0.05f, false)
	);
}

void DyingBlood::Update(float _DeltaTime) {
	if (Effect->IsUpdate() && Effect->CurAnimationEnd())
		Effect->Off();

	PrevTime += _DeltaTime;
}

void DyingBlood::SetType(GameBloodType _Type) {
	switch (_Type) {
	case GameBloodType::TINY:
		Renderer->SetTexture("effect_032_bloodstains_4.png");
		Renderer->SetFrame(rand() % Renderer->GetCurTexture()->GetCutCount());
		break;
	case GameBloodType::SMALL:
		Renderer->SetTexture("effect_032_bloodstains_2.png");
		Renderer->SetFrame(rand() % Renderer->GetCurTexture()->GetCutCount());
		break;
	case GameBloodType::MEDIUM:
		Renderer->SetTexture("effect_032_bloodstains_3.png");
		Renderer->SetFrame(rand() % Renderer->GetCurTexture()->GetCutCount());
		break;
	case GameBloodType::LARGE:
		Renderer->SetTexture("effect_032_bloodstains_1.png");
		Renderer->SetFrame(rand() % Renderer->GetCurTexture()->GetCutCount());
		break;
	case GameBloodType::HUGE:
		Renderer->SetTexture("effect_032_bloodstains_5.png");
		Renderer->SetFrame(rand() % Renderer->GetCurTexture()->GetCutCount());
		break;
	}
	Renderer->ScaleToCutTexture();
	Renderer->GetPixelData().MulColor.a = 0.6f;
}
 