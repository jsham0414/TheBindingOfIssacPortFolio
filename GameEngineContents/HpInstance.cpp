#include "PreCompile.h"
#include "HpInstance.h"

HpInstance::HpInstance() {
}

HpInstance::~HpInstance() {
}

void HpInstance::SetHp(int _Hp) {
	Hp = _Hp >= 2 ? 2 : _Hp;
	Hp = Hp > 0 ? Hp : 0;

	Renderer->ChangeFrameAnimation(to_string(Hp));
	Renderer->ScaleToCutTexture(Hp);
	Renderer->GetTransform().SetLocalPosition({ Index * (Renderer->GetTransformData().WorldScaling.x - 2.f), 0.f });
}

int HpInstance::GetIndex() {
	return Index;
}

void HpInstance::SetIndex(int _Index) {
	Index = _Index;
}

void HpInstance::Start() {
	Index = 0;

	GetTransform().SetWorldPosition({ -345.f, 224.f });
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);

	CreateFrameAnimation();
}

void HpInstance::CreateFrameAnimation() {
	//Renderer->
	Renderer->CreateFrameAnimationCutTexture("0", FrameAnimation_DESC("Heart.png", 0, 0, 0.1f, false));
	Renderer->CreateFrameAnimationCutTexture("1", FrameAnimation_DESC("Heart.png", 1, 1, 0.1f, false));
	Renderer->CreateFrameAnimationCutTexture("2", FrameAnimation_DESC("Heart.png", 2, 2, 0.1f, false));
}

void HpInstance::Update(float _DeltaTime) {

}
