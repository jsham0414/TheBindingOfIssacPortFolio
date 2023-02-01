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

PlayLevel::PlayLevel() 
{
}

PlayLevel::~PlayLevel() 
{
}

void PlayLevel::Start() {
	// 내가 직접 설정해줘야 합니다.
	GetMainCamera()->SetProjectionMode(CAMERAPROJECTIONMODE::Orthographic);

	// 컵헤드 올드필름 이미지 텍스처 폴더 이미지로 로드해서
	// 화면에 랜더링하세요.
	// 블랜드를 바꿔야 할겁니다.

	// GetMainCamera()->GetCameraRenderTarget()->AddEffect<GameEngineBlur>();

	// GameEngineDevice::GetBackBuffer()->AddEffect<GameEngineBlur>();

	// GetMainCamera()->GetCameraRenderTarget()->AddEffect<GameEngineBlur>();

	LoadTexture();

	if (false == GameEngineInput::GetInst()->IsKey("FreeCameaOnOff"))
	{
		GameEngineInput::GetInst()->CreateKey("FreeCameaOnOff", 'O');
	}


	{
	}

	{
		//Monster* actor = CreateActor<Monster>(OBJECTORDER::Monster);
		//actor->GetTransform().SetLocalPosition({ 300.0f, 0.0f, 100.0f });
		//actor->GetTransform().SetWorldScale(float4(1.0f, 1.f, 1.0f));
	}

}

void PlayLevel::LevelStartEvent()
{
	if (nullptr == Player::GetMainPlayer()) {
		Player* NewPlayer = CreateActor<Player>(OBJECTORDER::Player);
		//NewPlayer->SetLevelOverOn();
		//Horf* Temp = CreateActor<Horf>(OBJECTORDER::Monster);
		MapManagerInstance = CreateActor<MapManager>(OBJECTORDER::Manager);
		MapManagerInstance->RandomMapGenerate(this);
		UIManagerInstance = CreateActor<UIManager>(OBJECTORDER::Manager);
	}
}

void PlayLevel::LoadTexture() {
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
			if (Shaders[i].GetFileName() == "WoodDoor.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 4, 1);
			if (Shaders[i].GetFileName() == "issac_body_horizontal.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 5, 2);
			if (Shaders[i].GetFileName() == "issac_body_vertical.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 5, 2);
			if (Shaders[i].GetFileName() == "Horf.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 2);
			if (Shaders[i].GetFileName() == "Heart.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 1);
			if (Shaders[i].GetFileName() == "Stuffs.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 3, 1);
			if (Shaders[i].GetFileName() == "Number.png")
				GameEngineTexture::Cut(Shaders[i].GetFileName(), 10, 1);
		}
	}
}


void PlayLevel::Update(float _DeltaTime) 
{
	//GameEngineStatusWindow::AddDebugRenderTarget("BackBuffer", GameEngineDevice::GetBackBuffer());
	//GameEngineStatusWindow::AddDebugRenderTarget("MainCamera", GetMainCamera()->GetCameraRenderTarget());
	//GameEngineStatusWindow::AddDebugRenderTarget("UICamera", GetUICamera()->GetCameraRenderTarget());


	if (GameEngineInput::GetInst()->IsDown("FreeCameaOnOff"))
	{
		// ;
		GetMainCameraActor()->FreeCameraModeOnOff();
	}

	if (true == GameEngineInput::GetInst()->IsDown("LevelChange"))
	{
		GEngine::ChangeLevel("Login");
	}

}
void PlayLevel::End() {}
