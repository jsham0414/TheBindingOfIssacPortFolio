#include "PreCompile.h"
#include "MinimapIcon.h"
#include "MapManager.h"

MinimapIcon::MinimapIcon() {
}

MinimapIcon::~MinimapIcon() {
}

void MinimapIcon::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);

	auto Background = CreateComponent<GameEngineTextureRenderer>();
	Background->SetTexture("minimap1.png", 4);
	Background->ChangeCamera(CAMERAORDER::UICAMERA);
	Background->ScaleToCutTexture(4);
	Background->GetTransform().SetLocalPosition({ 0.f, 0.f, 100.f });
	Background->GetPixelData().MulColor.a = 0.5f;

	StateManager.CreateStateMember("Visit"
		, nullptr
		, std::bind(&MinimapIcon::VisitStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Cleared"
		, nullptr
		, std::bind(&MinimapIcon::ClearedStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Unknown"
		, nullptr
		, std::bind(&MinimapIcon::UnknownStart, this, std::placeholders::_1)
	);

	StateManager.ChangeState("Unknown");
}

void MinimapIcon::CreateFrameAnimation() {
}

void MinimapIcon::Update(float _DeltaTime) {

}

void MinimapIcon::VisitStart(const StateInfo& _Info) {
	Renderer->SetTexture("minimap1.png", 47);
	Renderer->ScaleToCutTexture();
}

void MinimapIcon::ClearedStart(const StateInfo& _Info) {
	Renderer->SetTexture("minimap1.png", 12);
	Renderer->ScaleToCutTexture();
}

void MinimapIcon::UnknownStart(const StateInfo& _Info) {
	Renderer->SetTexture("minimap1.png", 20);
	Renderer->ScaleToCutTexture();
}

void MinimapIcon::SetIndex(int _Index) {
	Index = _Index;
	GameRoomType RoomType = (GameRoomType)GetLevel()->GetMapManager()->GetMap(Index)->RoomType;

	GameEngineTextureRenderer* Icon;
	switch (RoomType) {
	case GameRoomType::Boss:
		Icon = CreateComponent<GameEngineTextureRenderer>();
		Icon->SetTexture("minimap1.png", 42);
		Icon->ChangeCamera(CAMERAORDER::UICAMERA);
		Icon->ScaleToCutTexture();
		break;
	case GameRoomType::Gold:
		Icon = CreateComponent<GameEngineTextureRenderer>();
		Icon->SetTexture("minimap1.png", 36);
		Icon->ChangeCamera(CAMERAORDER::UICAMERA);
		Icon->ScaleToCutTexture();
		break;
	case GameRoomType::Shop:
		Icon = CreateComponent<GameEngineTextureRenderer>();
		Icon->SetTexture("minimap1.png", 28);
		Icon->ChangeCamera(CAMERAORDER::UICAMERA);
		Icon->ScaleToCutTexture();
		break;
	}
}