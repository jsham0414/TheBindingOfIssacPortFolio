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
	int SubDigit = ValueToString.size() - RendererList.GetSize();

	// �ڸ����� ���� ���ų� ���� �����
	for (int i = 0; i < abs(SubDigit); i++) {
		if (SubDigit > 0) {
			auto NewRenderer = CreateComponent<GameEngineTextureRenderer>();
			NewRenderer->ChangeCamera(CAMERAORDER::UICAMERA);
			NewRenderer->SetFrameAni(Renderer->GetFrameAni());
			RendererList.AddNode(NewRenderer);
		} else if (SubDigit < 0) {
			RendererList.GetTail()->Data->Off();
		}
	}

	// �ڸ����� �°� ���ڸ� �־��ش�
	auto NumRenderer = RendererList.GetHead();
	for (int i = 0; i < ValueToString.size(); i++) {
		string NewNumber = to_string(ValueToString.c_str()[i]);
		auto _Renderer = NumRenderer->Data;
		_Renderer->On();
		_Renderer->ChangeFrameAnimation(NewNumber);
		int IntNum = ValueToString.c_str()[i] - (int)'0';
		_Renderer->ScaleToCutTexture(IntNum);
		_Renderer->GetTransform().SetLocalPosition({ i * (_Renderer->GetTransform().GetWorldScale().x - 2), 0.f });
	}
}

void UINumber::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);

	CreateFrameAnimation();
}

void UINumber::CreateFrameAnimation() {
	for (int i = 0; i < 10; i++) {
		Renderer->CreateFrameAnimationCutTexture(to_string(i), FrameAnimation_DESC("Number.png", i, i, 0.1f, false));
	}
}

void UINumber::Update(float _DeltaTime) {
}