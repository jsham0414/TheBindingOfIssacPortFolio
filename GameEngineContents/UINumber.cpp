#include "PreCompile.h"
#include "UINumber.h"

UINumber::UINumber() {
}

UINumber::~UINumber() {
}

void UINumber::ChangeValue(int _Value) {
	if (_Value < 0)
		return;

	Value = _Value;

	std::string ValueToString = std::to_string(Value);
	if (ValueToString.size() == 1)
		ValueToString = "0" + ValueToString;

	int SubDigit = ValueToString.size() - RendererList.GetSize();

	// 자릿수에 맞춰 끄거나 새로 만든다
	for (int i = 0; i < abs(SubDigit); i++) {
		if (SubDigit > 0) {
			auto NewRenderer = CreateComponent<GameEngineTextureRenderer>();
			NewRenderer->ChangeCamera(CAMERAORDER::UICAMERA);
			NewRenderer->SetFrameAni(Renderer->GetFrameAni());
			NewRenderer->SetScaleRatio(1.f);
			RendererList.AddNode(NewRenderer);
		} else if (SubDigit < 0) {
			RendererList.GetTail()->Data->Off();
		}
	}

	// 자릿수에 맞게 숫자를 넣어준다
	auto NumRenderer = RendererList.GetHead();
	for (int i = 0; i < ValueToString.size(); i++) {
		string NewNumber(&ValueToString.c_str()[i]);
		auto _Renderer = NumRenderer->Data;
		_Renderer->On();
		int IntNum = ValueToString.c_str()[i] - (int)'0';
		_Renderer->ChangeFrameAnimation(to_string(IntNum));
		_Renderer->SetScaleRatio(1.f);
		_Renderer->ScaleToCutTexture(IntNum);
		_Renderer->GetTransform().SetLocalPosition({ 18 + i * (_Renderer->GetTransform().GetWorldScale().x - 2), 0.f });
		NumRenderer = NumRenderer->NextNode;
	}
}

void UINumber::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);
	Renderer->SetScaleRatio(1.f);

	CreateFrameAnimation();
}

void UINumber::CreateFrameAnimation() {
	for (int i = 0; i < 10; i++) {
		Renderer->CreateFrameAnimationCutTexture(to_string(i), FrameAnimation_DESC("Number.png", i, i, 0.1f, false));
	}
}

void UINumber::Update(float _DeltaTime) {
}
