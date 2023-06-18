#include "PreCompile.h"
#include "BossUI.h"

BossUI::BossUI() {
}

BossUI::~BossUI() {
}

void BossUI::SetHp(float _X) {
	Renderer->GetPixelData().Slice.x = _X;
}

void BossUI::Start() {
	GetTransform().SetWorldPosition({ 0.f, -200.f });

	auto Background = CreateComponent<GameEngineTextureRenderer>();
	Background->ChangeCamera(CAMERAORDER::UICAMERA);
	Background->SetTexture("ui_bosshealthbar.png", 1);
	Background->ScaleToCutTexture();

	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);
	Renderer->SetTexture("ui_bosshealthbar.png", 0);
	Renderer->ScaleToCutTexture();
	Renderer->GetPixelData().Slice.x = 1;
	Renderer->GetPixelData().FillType = 1;
}

void BossUI::Update(float _DeltaTime) {
}
