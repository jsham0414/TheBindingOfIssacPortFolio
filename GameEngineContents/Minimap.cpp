#include "PreCompile.h"
#include "Minimap.h"
#include "MinimapIcon.h"

Minimap::Minimap() {
}

Minimap::~Minimap() {
}

void Minimap::Start() {
	GetTransform().SetWorldPosition( {
		GameEngineWindow::GetScale().x / 2.f - 106,
		GameEngineWindow::GetScale().y / 2.f - 82
	} );
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->ChangeCamera(CAMERAORDER::UICAMERA);
	Renderer->SetScaleRatio(1.9f);
	Renderer->SetTexture("minimap.png");
	Renderer->ScaleToTexture();
	Renderer->GetTransform().SetLocalPosition({ 0.f, 2.f });

	MoveMap(45);
}

void Minimap::CreateFrameAnimation() {
}

void Minimap::Update(float _DeltaTime) {
}

void Minimap::MoveMap(int _Index) {
	MinimapIcon* VisitedIcon = nullptr;
	if (IconRenderer.find(_Index) == IconRenderer.end()) {
		IconRenderer[_Index] = GetLevel()->CreateActor<MinimapIcon>();
		IconRenderer[_Index]->SetIndex(_Index);
		IconRenderer[_Index]->SetParent(this);
		CurMapIndex = _Index;
	}

	IconRenderer[CurMapIndex]->StateManager.ChangeState("Cleared");
	VisitedIcon = IconRenderer[_Index];
	VisitedIcon->StateManager.ChangeState("Visit");

	int Dir[4] = { -10, -1, 1, 10 };
	for (int i = 0; i < 4; i++) {
		int CheckRoomIndex = _Index + Dir[i];
		int MapType = GetLevel()->GetMapManager()->GetRoomType(CheckRoomIndex);
		if (MapType == -1)
			continue;

		if (IconRenderer.find(CheckRoomIndex) == IconRenderer.end()) {
			IconRenderer[CheckRoomIndex] = GetLevel()->CreateActor<MinimapIcon>();
			IconRenderer[CheckRoomIndex]->SetIndex(CheckRoomIndex);
			IconRenderer[CheckRoomIndex]->SetParent(this);
		}
	}
	CurMapIndex = _Index;

	for (auto Iter = IconRenderer.begin(); Iter != IconRenderer.end(); Iter++) {
		if (Iter->second == nullptr)
			continue;

		auto& Icon = Iter->second;
		int Index = Iter->first - CurMapIndex + 45;

		if (abs((Iter->first % 10) - (CurMapIndex % 10)) > 2 ||
			abs((Iter->first / 10) - (CurMapIndex / 10)) > 2)
			Icon->Off();
		else
			Icon->On();

		float4 a = {
			((Index % 10) * 16.f),
			((Index >= 10 ? Index / 10 : Index) * -16.f),
		};
		float4 b = {
			((CurMapIndex % 10) * 16.f),
			((CurMapIndex >= 10 ? CurMapIndex / 10 : CurMapIndex) * -16.f),
		};

		float4 NewPos = {
			((Index % 10) * 16.f) - 5 * 16.f,
			(((Index != 0) ? (Index / 10) : 0) * -16.f) - (4 * -16.f),
			200.f
		};
		Icon->GetTransform().SetLocalPosition(NewPos);
	}
}