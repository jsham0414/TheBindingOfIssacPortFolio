#include "PreCompile.h"
#include "StuffGuider.h"

StuffGuider::StuffGuider() {
}

StuffGuider::~StuffGuider() {
}

void StuffGuider::ChangeValue(int _Value) {
	NumberInstance->ChangeValue(_Value);
}

void StuffGuider::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);

	NumberInstance = GetLevel()->CreateActor<UINumber>();
	NumberInstance->SetParent(this);
}

void StuffGuider::CreateFrameAnimation() {
}

void StuffGuider::Update(float _DeltaTime) {
}
