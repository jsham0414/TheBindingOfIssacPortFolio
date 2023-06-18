#include "PreCompile.h"
#include "LoadingUI.h"

LoadingUI::LoadingUI() {
}

LoadingUI::~LoadingUI() {
}

void LoadingUI::Start() {
	SetLevelOverOn();

	Renderer = CreateComponent<GameEngineTextureRenderer>();
	LoadingImage = CreateComponent<GameEngineTextureRenderer>();

	Renderer->SetTexture("bgblack.png");
	Renderer->SetScaleRatio(100.f);
	Renderer->ScaleToTexture();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);

	int RandomNumber = rand() % 56 + 1;
	
	std::string SubDigit;
	for (int i = 0; i < 3 - to_string(RandomNumber).size(); i++) {
		SubDigit.push_back('0');
	}
	SubDigit += to_string(RandomNumber);
	
	LoadingImage->SetTexture("loadimages-" + SubDigit + ".png");
	LoadingImage->ScaleToTexture();
	LoadingImage->ChangeCamera(CAMERAORDER::UICAMERA);

	StateManager.CreateStateMember("FadeIn",
		std::bind(&LoadingUI::FadeInUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&LoadingUI::FadeInStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Loading",
		std::bind(&LoadingUI::LoadingUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&LoadingUI::LoadingStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("FadeOut",
		std::bind(&LoadingUI::FadeOutUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&LoadingUI::FadeOutStart, this, std::placeholders::_1)
	);

	StateManager.ChangeState("FadeIn");
}

void LoadingUI::Update(float _DeltaTime) {
	StateManager.Update(_DeltaTime);
}

void LoadingUI::FadeInStart(const StateInfo& _Info) {
	StartTime = GetTickCount64();
	FadeTime = 2.f;
	Renderer->GetPixelData().MulColor.a = 0.f;
	LoadingImage->GetPixelData().MulColor.a = 0.f;
}

void LoadingUI::FadeInUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (GetTickCount64() - StartTime) / 1000.f / FadeTime;
	Duration = fmin(1.f, Duration);
	Renderer->GetPixelData().MulColor.a = std::lerp(0.f, 1.f, GameEngineMath::EaseOutQuint(Duration));
	LoadingImage->GetPixelData().MulColor.a = std::lerp(0.f, 1.f, GameEngineMath::EaseOutQuint(Duration));

	if (Duration >= 1.f) {
		StateManager.ChangeState("Loading");
	}
}

void LoadingUI::LoadingStart(const StateInfo& _Info) {
	// 나중에 프로젝트가 커졌을 때 켜도록 하자
	//GEngine::GetLevel("Play")->PreLoading()

	GEngine::ChangeLevel("Play");
}

void LoadingUI::LoadingUpdate(float _DeltaTime, const StateInfo& _Info) {
	std::string asd = GetLevel()->GetNameConstPtr();
	if (GetLevel()->CompareName("Play")) {
		GEngine::GetLevel("Play")->SetPause(true);
		StateManager.ChangeState("FadeOut");
	}
}

void LoadingUI::FadeOutStart(const StateInfo& _Info) {
	StartTime = GetTickCount64();
	FadeTime = 2.f;
	Renderer->GetPixelData().MulColor.a = 1.f;
	LoadingImage->GetPixelData().MulColor.a = 1.f;
}

void LoadingUI::FadeOutUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (GetTickCount64() - StartTime) / 1000.f / FadeTime;
	Duration = fmin(1.f, Duration);
	Renderer->GetPixelData().MulColor.a = std::lerp(1.f, 0.f, GameEngineMath::EaseOutQuint(Duration));
	LoadingImage->GetPixelData().MulColor.a = std::lerp(1.f, 0.f, GameEngineMath::EaseOutQuint(Duration)); 

	if (Duration >= 0.5f && GetLevel()->GetPause()) {
		GetLevel()->SetPause(false);
	}
}
