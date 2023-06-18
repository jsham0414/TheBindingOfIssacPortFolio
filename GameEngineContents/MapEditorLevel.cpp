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
#include "ItemHolder.h"
#include <GameEngineContents/Monstro.h>
#include <GameEngineContents/Gaper.h>

MapEditorLevel::MapEditorLevel()
{
	MapOffset = { -142.f, -87.f };
	RoomWidth = 13;
	RoomHeight = 7;
	TileSize = 52;
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
	CreateDummy<Horf>();
	CreateDummy<ItemHolder>();
	CreateDummy<Monstro>();
	CreateDummy<Gaper>();
}

void MapEditorLevel::Update(float _DeltaTime)
{
	float4 MousePos = GetMainCamera()->GetMouseScreenPosition();
	MousePos += MapOffset;
	float fX = (MousePos.x / TileSize);
	float fY = (MousePos.y / TileSize);

	int _X = static_cast<int>(floorf(fX));
	int _Y = static_cast<int>(floorf(fY));

	int Index = _X + _Y * RoomWidth;

	if (!(0 <= _X && _X < RoomWidth))
		return;

	if (!(0 <= _Y && _Y < RoomHeight))
		return;

	if (GameEngineInput::GetInst()->IsUp("LeftMouse"))
		ObjectBatcher->BatchObject(Index, TileMapEditorGUI->BatchInfos[TileMapEditorGUI->Selected]);
	
	if (GameEngineInput::GetInst()->IsUp("RightMouse"))
		ObjectBatcher->DeleteObject(Index);
}

void MapEditorLevel::End()
{
}

bool MapEditorLevel::LoadTexture() {
	return GEngine::GetLevel("Play")->LoadTexture();
}
