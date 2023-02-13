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

PlayLevel::PlayLevel() {
}

PlayLevel::~PlayLevel() {
}

void PlayLevel::Start() {
	GetMainCamera()->SetProjectionMode(CAMERAPROJECTIONMODE::Orthographic);
	// 로딩 구현 할 정도의 크기가 아니라 시작할 때 몽땅 해놓고 로딩이 되는 것 처럼 보이게 함
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
		}
	}

	return true;
}


void PlayLevel::Update(float _DeltaTime) {
	//GameEngineStatusWindow::AddDebugRenderTarget("BackBuffer", GameEngineDevice::GetBackBuffer());
	//GameEngineStatusWindow::AddDebugRenderTarget("MainCamera", GetMainCamera()->GetCameraRenderTarget());
	//GameEngineStatusWindow::AddDebugRenderTarget("UICamera", GetUICamera()->GetCameraRenderTarget());

}
void PlayLevel::End() {}
