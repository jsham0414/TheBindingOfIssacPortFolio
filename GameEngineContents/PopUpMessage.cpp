#include "PreCompile.h"
#include "PopUpMessage.h"

PopUpMessage::PopUpMessage() {
	StartX = -1000.f;
	EndX = 1000.f;
}

PopUpMessage::~PopUpMessage() {
}

void PopUpMessage::Start() {
	GetTransform().SetWorldPosition({StartX, 150.f});

	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->SetTexture("effect_024_streak.png");
	Renderer->SetScaleRatio(1.8f);
	Renderer->ScaleToTexture();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);

	FontRenderer["Title"] = CreateComponent<GameEngineFontRenderer>();
	FontRenderer["Title"]->SetText("", "SEBANGGothic.ttf");
	FontRenderer["Title"]->ChangeCamera(CAMERAORDER::UICAMERA);
	FontRenderer["Title"]->SetScreenPostion({ GameEngineWindow::GetScale().x / 2.f + StartX, GetTransformData().WorldPosition.y - 40.f });
	FontRenderer["Title"]->SetSize(36.f);
	FontRenderer["Title"]->SetTopAndBotSort(TopAndBotSort::VCENTER);
	FontRenderer["Title"]->SetLeftAndRightSort(LeftAndRightSort::CENTER);

	FontRenderer["Context"] = CreateComponent<GameEngineFontRenderer>();
	FontRenderer["Context"]->SetText("", "SEBANGGothic.ttf");
	FontRenderer["Context"]->ChangeCamera(CAMERAORDER::UICAMERA);
	FontRenderer["Context"]->SetScreenPostion({ GameEngineWindow::GetScale().x / 2.f + StartX, GetTransformData().WorldPosition.y - 10.f });
	FontRenderer["Context"]->SetSize(20.f);
	FontRenderer["Context"]->SetTopAndBotSort(TopAndBotSort::VCENTER);
	FontRenderer["Context"]->SetLeftAndRightSort(LeftAndRightSort::CENTER);

	Renderer->Off();
	FontRenderer["Title"]->Off();
	FontRenderer["Context"]->Off();

	StateManager.CreateStateMember("Moving",
		std::bind(&PopUpMessage::MovingUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&PopUpMessage::MovingStart, this, std::placeholders::_1),
		nullptr
	);

	StateManager.CreateStateMember("Waiting",
		std::bind(&PopUpMessage::WaitingUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&PopUpMessage::WaitingStart, this, std::placeholders::_1),
		nullptr
	);

	StateManager.CreateStateMember("End",
		std::bind(&PopUpMessage::EndUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&PopUpMessage::EndStart, this, std::placeholders::_1),
		nullptr
	);
}

void PopUpMessage::Update(float _DeltaTime) {
	StateManager.Update(_DeltaTime);
}

void PopUpMessage::Play() {
	Renderer->On();
	FontRenderer["Title"]->On();
	FontRenderer["Context"]->On();
	StateManager.ChangeState("Moving");
}

void PopUpMessage::MovingStart(const StateInfo& _Info) {
	StartTime = GetTickCount64();
}

using ULL = ULONGLONG;
void PopUpMessage::MovingUpdate(float _DeltaTime, const StateInfo& _Info) {
	ULL a = GetTickCount64();
	float Duration = (GetTickCount64() - StartTime) / 1000.f;

	if (Duration >= 1.f) {
		StateManager.ChangeState("Waiting");
		return;
	}

	float NewX = std::lerp(StartX, 0.f, GameEngineMath::EaseOutQuint(fmin(1.f, Duration)));
	float4 NewPos = GetTransformData().WorldPosition;
	float4 TitlePos = FontRenderer["Title"]->GetScreenPostion();
	float4 ContextPos = FontRenderer["Context"]->GetScreenPostion();
	NewPos.x = NewX;
	TitlePos.x = GameEngineWindow::GetScale().x / 2.f + NewX;
	ContextPos.x = GameEngineWindow::GetScale().x / 2.f + NewX;
	GetTransform().SetWorldPosition(NewPos);
	FontRenderer["Title"]->SetScreenPostion(TitlePos);
	FontRenderer["Context"]->SetScreenPostion(ContextPos);
}

void PopUpMessage::WaitingStart(const StateInfo& _Info) {
	StartTime = GetTickCount64();
}

void PopUpMessage::WaitingUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (GetTickCount64() - StartTime) / 1000.f / 0.5f;

	if (Duration > 1.f) {
		StateManager.ChangeState("End");
		return;
	}
}

void PopUpMessage::EndStart(const StateInfo& _Info) {
	StartTime = GetTickCount64();
}

void PopUpMessage::EndUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (GetTickCount64() - StartTime) / 1000.f;

	if (Duration > 1.f) {
		Death();
		return;
	}

	float NewX = std::lerp(0.f, EndX, GameEngineMath::EaseInQuint(fmin(1.f, Duration)));
	float4 NewPos = GetTransformData().WorldPosition;
	float4 TitlePos = FontRenderer["Title"]->GetScreenPostion();
	float4 ContextPos = FontRenderer["Context"]->GetScreenPostion();
	NewPos.x = NewX;
	TitlePos.x = GameEngineWindow::GetScale().x / 2.f + NewX;
	ContextPos.x = GameEngineWindow::GetScale().x / 2.f + NewX;
	GetTransform().SetWorldPosition(NewPos);
	FontRenderer["Title"]->SetScreenPostion(TitlePos);
	FontRenderer["Context"]->SetScreenPostion(ContextPos);
}

void PopUpMessage::SetTitle(const std::string& _Text, const std::string& _Font) {
	FontRenderer["Title"]->SetText(_Text, _Font);
}

void PopUpMessage::SetText(const std::string& _Text, const std::string& _Font) {
	FontRenderer["Context"]->SetText(_Text, _Font);
}