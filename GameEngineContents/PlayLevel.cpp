#include "PreCompile.h"
#include "PlayLevel.h"
#include "GlobalContentsValue.h"
#include <GameEngineCore/GameEngineCameraActor.h>
#include <GameEngineBase/GameEngineInput.h>

#include <GameEngineCore/GameEngineBlur.h>
#include "Player.h"
#include "Monster.h"
#include "UIMaster.h"
#include "Horf.h"
#include "MapManager.h"
#include "UIManager.h"
#include <GameEngineContents/Monstro.h>
#include <GameEngineContents/PopUpMessage.h>
#include <GameEngineContents/Gaper.h>
#include "HoldedItem.h"

PlayLevel::PlayLevel() {
	SetName("Play");
}

PlayLevel::~PlayLevel() {
}

void PlayLevel::Start() {
	GetMainCamera()->SetProjectionMode(CAMERAPROJECTIONMODE::Orthographic);

	PreLoading();
}

void PlayLevel::LevelStartEvent() {
	if (nullptr == Player::GetMainPlayer()) {
		Player* NewPlayer = CreateActor<Player>(OBJECTORDER::Player);
		MapManagerInstance = CreateActor<MapManager>(OBJECTORDER::Manager);
		MapManagerInstance->RandomMapGenerate(this);
		UIManagerInstance = CreateActor<UIManager>(OBJECTORDER::Manager);
	}
}

bool PlayLevel::PreLoading() {
	if (FAILED(LoadTexture()))
		return false;

	LevelStartEvent();

	return true;
}

bool PlayLevel::LoadTexture() {
	GameEngineDirectory Dir;
	Dir.MoveParentToExitsChildDirectory("ContentsResources");
	Dir.Move("ContentsResources");
	Dir.Move("Texture");
	Dir.Move("Play");

	if (nullptr == GameEngineTexture::Find("issac_head.png")) {
		std::vector<GameEngineFile> Shaders = Dir.GetAllFile();

		for (size_t i = 0; i < Shaders.size(); i++) {
			if (GameEngineTexture::Find(Shaders[i].GetFileName()) != nullptr)
				continue;

			GameEngineTexture::Load(Shaders[i].GetFullPath());

			//#define ASASD(path) if (nullptr == GameEngineTexture::Find(path) && GameEngineString::ToLowerReturn(GameEnginePath::GetFileName(Shaders[i].GetFullPath()) + GameEnginePath::GetExtension(Shaders[i].GetFullPath()))

			if (Shaders[i].GetFileName() == "issac_head.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 4);
			if (Shaders[i].GetFileName() == "issac_body_horizontal.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 8, 2);
			if (Shaders[i].GetFileName() == "issac_body_vertical.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 10, 1);
			if (Shaders[i].GetFileName() == "WoodDoor.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 4, 1);
			if (Shaders[i].GetFileName() == "Horf.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
			if (Shaders[i].GetFileName() == "Heart.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 1);
			if (Shaders[i].GetFileName() == "Stuffs.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 1);
			if (Shaders[i].GetFileName() == "Number.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 10, 1);
			if (Shaders[i].GetFileName() == "TearSplash.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 6, 1);
			if (Shaders[i].GetFileName() == "effect_015_tearpoofb.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 4, 4);
			if (Shaders[i].GetFileName() == "tears.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 8, 4);
			if (Shaders[i].GetFileName() == "Issac_hit.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
			if (Shaders[i].GetFileName() == "pickup_016_bomb.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 5, 6);
			if (Shaders[i].GetFileName() == "effect_029_explosion.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 4, 4);
			if (Shaders[i].GetFileName() == "01_basement.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 3);
			if (Shaders[i].GetFileName() == "door_01_normaldoor.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 5);
			if (Shaders[i].GetFileName() == "door_02_treasureroomdoor.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 4);
			if (Shaders[i].GetFileName() == "door_10_bossroomdoor.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 4);
			if (Shaders[i].GetFileName() == "levelitem_001_itemaltar.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 3);
			if (Shaders[i].GetFileName() == "boss_004_monstro.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 5, 2);
			if (Shaders[i].GetFileName() == "minimap1.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 8, 10);
			if (Shaders[i].GetFileName() == "minimap1.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 8, 10);
			if (Shaders[i].GetFileName() == "effect_002_bloodpoof.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 4, 4);
			if (Shaders[i].GetFileName() == "effect_032_bloodstains_1.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 1, 6);
			if (Shaders[i].GetFileName() == "effect_032_bloodstains_2.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 3);
			if (Shaders[i].GetFileName() == "effect_032_bloodstains_3.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
			if (Shaders[i].GetFileName() == "effect_032_bloodstains_4.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 1, 12);
			if (Shaders[i].GetFileName() == "effect_032_bloodstains_5.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 1, 2);
			if (Shaders[i].GetFileName() == "monster_017_gaper.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
			if (Shaders[i].GetFileName() == "effect_016_bloodpool1.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 6);
			if (Shaders[i].GetFileName() == "pickup_001_heart.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 3);
			if (Shaders[i].GetFileName() == "pickup_002_coin.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 8, 3);
			if (Shaders[i].GetFileName() == "pickup_003_key.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 4, 2);
			if (Shaders[i].GetFileName() == "ui_bosshealthbar.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 1, 2);
			if (Shaders[i].GetFileName() == "slot_001_machine_card.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 7, 2);
			if (Shaders[i].GetFileName() == "slot_001_machine.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
			
		}
	}

	LoadFont();
	return true;
}

bool PlayLevel::LoadFont() {
	GameEngineDirectory Dir;
	Dir.MoveParentToExitsChildDirectory("GameEngineResources");
	Dir.Move("GameEngineResources");
	Dir.Move("Font");

	std::vector<GameEngineFile> Fonts = Dir.GetAllFile();
	for (size_t i = 0; i < Fonts.size(); i++) {
		if (GameEngineFont::Find(Fonts[i].GetFileName()) != nullptr)
			continue;

		GameEngineFont::Load(Fonts[i].GetFullPath());
	}

	return true;
}


void PlayLevel::Update(float _DeltaTime) {
	//GameEngineStatusWindow::AddDebugRenderTarget("BackBuffer", GameEngineDevice::GetBackBuffer());
	//GameEngineStatusWindow::AddDebugRenderTarget("MainCamera", GetMainCamera()->GetCameraRenderTarget());
	//GameEngineStatusWindow::AddDebugRenderTarget("UICamera", GetUICamera()->GetCameraRenderTarget());

}
void PlayLevel::End() {}
