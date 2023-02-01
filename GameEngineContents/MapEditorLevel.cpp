#include "PreCompile.h"
#include "MapEditorLevel.h"
#include "LoginUI.h"
#include "Enums.h"
#include <GameEngineCore/GameEngineCameraActor.h>
#include <GameEngineBase/GameEngineInput.h>
#include "TileMapActor.h"
#include "MapEditorWindow.h"
#include "MapObjectBatcher.h"
#include "Horf.h"

MapEditorLevel::MapEditorLevel()
{
}

MapEditorLevel::~MapEditorLevel()
{
}


void MapEditorLevel::Start() {
	GameEngineInput::GetInst()->CreateKey("LeftMouse", VK_LBUTTON);
	GameEngineInput::GetInst()->CreateKey("RightMouse", VK_RBUTTON);

	//TileMap = CreateActor<TileMapActor>(OBJECTORDER::TileMap);
}

void MapEditorLevel::LevelStartEvent() {
	GetMainCameraActorTransform().SetWorldPosition(float4::ZERO + float4::BACK * 100.0f);

	LoadTexture();

	// 2번 로딩 안되도록
	if (nullptr == GameEngineTexture::Find("Frame_0.png"))
	{
		GameEngineDirectory Dir;
		Dir.MoveParentToExitsChildDirectory("ContentsResources");
		Dir.Move("ContentsResources");
		Dir.Move("Texture");
		Dir.Move("Map");

		std::vector<GameEngineFile> Ress = Dir.GetAllFile();

		for (size_t i = 0; i < Ress.size(); i++)
		{
			GameEngineTexture::Load(Ress[i].GetFullPath());
		}

		LoadAllObject();

		ObjectBatcher = CreateActor<MapObjectBatcher>(OBJECTORDER::TileMap);

		TileMapEditorGUI = GameEngineGUI::CreateGUIWindow<MapEditorWindow>("MapEditorGui", this);
		TileMapEditorGUI->SetObjectBatcher(ObjectBatcher);
	}
}

void MapEditorLevel::LoadAllObject() {
	CreateActor<Horf>(OBJECTORDER::TileMapObject);
}

void MapEditorLevel::Update(float _DeltaTime)
{
	float4 MousePos = GetMainCamera()->GetMouseScreenPosition();
	MousePos += {-142.f, -87.f};
	float fX = (MousePos.x / 52);
	float fY = (MousePos.y / 52);

	int _X = static_cast<int>(floorf(fX));
	int _Y = static_cast<int>(floorf(fY));

	int Index = _X + _Y * 13;

	int MaxIndex = 7 * 13;


	if (!(0 <= _X && _X < 13))
		return;

	if (!(0 <= _Y && _Y < 7))
		return;


	if (GameEngineInput::GetInst()->IsUp("LeftMouse")) {
		ObjectBatcher->BatchObject(Index, TileMapEditorGUI->BatchInfos[TileMapEditorGUI->Selected]);
	}

	if (GameEngineInput::GetInst()->IsUp("RightMouse")) {
		ObjectBatcher->DeleteObject(Index);
	}

	// 그림하나 띄우려고 액터 하나 만드는 건 너무나도 귀찮은 일이기 때문에 만들어 봤습니다.
	//GameEngineDebug::DrawTexture("PivotTile.png", { 0,0,0,1 }, { 0,0,0});
}

void MapEditorLevel::End()
{
}

void MapEditorLevel::LoadTexture() {
	GameEngineDirectory Dir;
	Dir.MoveParentToExitsChildDirectory("ContentsResources");
	Dir.Move("ContentsResources");
	Dir.Move("Texture");
	Dir.Move("Play");

	if (nullptr == GameEngineTexture::Find("issac_head.png")) {
		std::vector<GameEngineFile> Shaders = Dir.GetAllFile();

		for (size_t i = 0; i < Shaders.size(); i++) {
			GameEngineTexture::Load(Shaders[i].GetFullPath());

			if (Shaders[i].GetFileName() == "issac_head.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 4);
			if (Shaders[i].GetFileName() == "issac_body_horizontal.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 5, 2);
			if (Shaders[i].GetFileName() == "issac_body_vertical.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 5, 2);
			if (Shaders[i].GetFileName() == "Horf.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
			if (Shaders[i].GetFileName() == "room_001.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
		}
	}
}
