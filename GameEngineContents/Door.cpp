#include "PreCompile.h"
#include "Door.h"
#include <GameEngineCore/GameEngineTextureRenderer.h>

Door::Door() {
	SetName("Door");
	OpenTime = 0.25f;
	Open = false;
}

Door::~Door() {
	// !!

	Spawn->DetachTransform();
	SAFE_DELETE(Spawn);
}

void Door::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Collision = CreateComponent<GameEngineCollision>();
	Collision->ChangeOrder(OBJECTORDER::Wall);

	StateManager.CreateStateMember("Open"
		, std::bind(&Door::OpenUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Door::OpenStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Close"
		, std::bind(&Door::CloseUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Door::CloseStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Wall"
		, nullptr
		, std::bind(&Door::WallStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("FakeOpen"
		, std::bind(&Door::FakeOpenUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&Door::FakeOpenStart, this, std::placeholders::_1)
	);
}

void Door::Initialize() {
	Spawn = new GameEngineTransform();
	Spawn->SetParentTransform(GetTransform());
	Spawn->SetLocalPosition({ 0, -40 });
	CreateFrameAnimation();
}

bool Door::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {


	return true;
}

void Door::CreateFrameAnimation() {
	std::string FileName = "door_01_normaldoor.png";
	switch (Type) {
	case GameRoomType::Room:
		FileName = "door_01_normaldoor.png";
		break;
	case GameRoomType::Gold:
		FileName = "door_02_treasureroomdoor.png";
		break;
	case GameRoomType::Boss:
		FileName = "door_10_bossroomdoor.png";
		DoorRenderer["Floor"] = CreateComponent<GameEngineTextureRenderer>();
		DoorRenderer["Floor"]->SetTexture(FileName, 7);
		DoorRenderer["Floor"]->ScaleToCutTexture();
		DoorRenderer["Floor"]->GetTransform().SetLocalPosition({ 0.f, -64.f, 3001.f });
		break;
	case GameRoomType::Shop:
		FileName = "door_01_normaldoor.png";
		break;
	}

	Renderer->SetTexture(FileName, 0);
	Renderer->ScaleToCutTexture();
	Renderer->GetTransform().SetLocalPosition({ 0.f, 0.f, 2997.f });
	DoorRenderer["Main"] = Renderer;

	// ±×¸²ÀÚ
	DoorRenderer["Shadow"] = CreateComponent<GameEngineTextureRenderer>();
	DoorRenderer["Shadow"]->SetTexture(FileName, 1);
	DoorRenderer["Shadow"]->ScaleToCutTexture();
	DoorRenderer["Shadow"]->GetTransform().SetLocalPosition({ 0.f, 0.f, 2999.f});

	// ¿ÞÂÊ ¹®Â¦
	DoorRenderer["Left"] = CreateComponent<GameEngineTextureRenderer>();
	DoorRenderer["Left"]->SetTexture(FileName, 2);
	DoorRenderer["Left"]->ScaleToCutTexture();
	DoorRenderer["Left"]->GetTransform().SetLocalPosition({ 0.f, 0.f, 2998.f });


	// ¿À¸¥ÂÊ ¹®Â¦
	DoorRenderer["Right"] = CreateComponent<GameEngineTextureRenderer>();
	DoorRenderer["Right"]->SetTexture(FileName, 3);
	DoorRenderer["Right"]->ScaleToCutTexture();
	DoorRenderer["Right"]->GetTransform().SetLocalPosition({ 0.f, 0.f, 2998.f });

	DoorSize = DoorRenderer["Right"]->GetTransform().GetLocalScale();

	Collision->GetTransform().SetWorldScale(float4(Renderer->GetTransformData().WorldScaling.y, Renderer->GetTransformData().WorldScaling.y));
	float4 Color = float4::RED;
	Color.a = 0.2f;
	Collision->SetDebugSetting(CollisionType::CT_AABB2D, Color);
	Collision->GetTransform().SetLocalPosition({ 0, -20 });

	Collision->GetTransform().SetStatic(true);

	//Renderer->SetTexture(FileName, )

	StateManager.ChangeState("Open");
}

void Door::SetRendererState(bool _State) {
	for (auto& _Renderer : DoorRenderer) {
		_State ? _Renderer.second->On() : _Renderer.second->Off();
	}
}

void Door::Update(float _DeltaTime) {
	if (StateManager.GetCurStateName() == "")
		return;

	StateManager.Update(_DeltaTime);
}

void Door::SetType(GameRoomType _Type) {
	Type = _Type;
}

void Door::CloseStart(const StateInfo& _Info) {
	StartTime = GetTickCount64();
	
	Collision->GetTransform().SetLocalPosition({ 0, 13 });

	DoorRenderer["Left"]->On();
	DoorRenderer["Right"]->On();

	DoorRenderer["Left"]->GetTransform().SetLocalScale({ DoorSize.x, DoorSize.y });
	DoorRenderer["Right"]->GetTransform().SetLocalScale({ DoorSize.x, DoorSize.y });

	Open = false;
}

void Door::CloseUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (GetTickCount64() - StartTime) / 1000.f / OpenTime;
	Duration = fmin(1.f, Duration);

	float NewX = std::lerp(12.f, 0.f, GameEngineMath::EaseOutQuint(Duration));
	DoorRenderer["Left"]->GetTransform().SetLocalPosition({ -NewX, 0.f, DoorRenderer["Left"]->GetTransform().GetLocalPosition().z });
	DoorRenderer["Right"]->GetTransform().SetLocalPosition({ NewX, 0.f, DoorRenderer["Right"]->GetTransform().GetLocalPosition().z });

	if (Type == GameRoomType::Boss) {
		DoorRenderer["Floor"]->GetPixelData().MulColor.a = fmax(0.f, 1.f - Duration * 2.f);
	}
}

void Door::OpenStart(const StateInfo& _Info) {
	Collision->GetTransform().SetLocalPosition({ 0, 50 });

	StartTime = GetTickCount64();
}

void Door::FakeOpenStart(const StateInfo& _Info) {
	Open = false;

	Collision->GetTransform().SetLocalPosition({ 0, 13 });
	StartTime = GetTickCount64();
}

void Door::FakeOpenUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (GetTickCount64() - StartTime) / 1000.f / OpenTime;
	Duration = fmin(1.f, Duration);

	float NewX = std::lerp(0.f, 20.f, GameEngineMath::EaseOutQuint(Duration));
	DoorRenderer["Left"]->GetTransform().SetLocalPosition({ -NewX, 0.f, DoorRenderer["Left"]->GetTransform().GetLocalPosition().z });
	DoorRenderer["Right"]->GetTransform().SetLocalPosition({ NewX, 0.f, DoorRenderer["Right"]->GetTransform().GetLocalPosition().z });

	if (Duration >= 0.22f) {
		DoorRenderer["Left"]->GetTransform().SetLocalScale({ DoorSize.x / 2.f, DoorSize.y });
		DoorRenderer["Right"]->GetTransform().SetLocalScale({ DoorSize.x / 2.f, DoorSize.y });
	}

	if (Type == GameRoomType::Boss) {
		DoorRenderer["Floor"]->GetPixelData().MulColor.a = fmin(1.f, Duration * 2.f);
	}

	if (Duration >= 0.5f) {
		if (Type == GameRoomType::Boss) {
			DoorRenderer["Left"]->GetTransform().SetLocalScale({ DoorSize.x / 3.f, DoorSize.y });
			DoorRenderer["Right"]->GetTransform().SetLocalScale({ DoorSize.x / 3.f, DoorSize.y });
		} else {
			DoorRenderer["Left"]->Off();
			DoorRenderer["Right"]->Off();
		}
	}
}

void Door::WallStart(const StateInfo& _Info) {
	Collision->GetTransform().SetLocalPosition({ 0, 13 });
	SetRendererState(false);
}

void Door::OpenUpdate(float _DeltaTime, const StateInfo& _Info) {
	float Duration = (GetTickCount64() - StartTime) / 1000.f / OpenTime;
	Duration = fmin(1.f, Duration);

	if (_Info.PrevState == "FakeOpen")
		Duration = 1.f;

	float NewX = std::lerp(0.f, 20.f, GameEngineMath::EaseOutQuint(Duration));
	DoorRenderer["Left"]->GetTransform().SetLocalPosition({ -NewX, 0.f, DoorRenderer["Left"]->GetTransform().GetLocalPosition().z });
	DoorRenderer["Right"]->GetTransform().SetLocalPosition({ NewX, 0.f, DoorRenderer["Right"]->GetTransform().GetLocalPosition().z });

	if (Duration >= 0.22f) {
		DoorRenderer["Left"]->GetTransform().SetLocalScale({ DoorSize.x / 2.f, DoorSize.y });
		DoorRenderer["Right"]->GetTransform().SetLocalScale({ DoorSize.x / 2.f, DoorSize.y });
	}

	if (Type == GameRoomType::Boss) {
		DoorRenderer["Floor"]->GetPixelData().MulColor.a = fmin(1.f, Duration * 2.f);
	}

	if (Duration >= 0.5f) {
		Open = true;

		if (Type == GameRoomType::Boss) {
			DoorRenderer["Left"]->GetTransform().SetLocalScale({ DoorSize.x / 3.f, DoorSize.y });
			DoorRenderer["Right"]->GetTransform().SetLocalScale({ DoorSize.x / 3.f, DoorSize.y });
		} else {
			DoorRenderer["Left"]->Off();
			DoorRenderer["Right"]->Off();
		}
	}
}
