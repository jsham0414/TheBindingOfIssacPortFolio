#include "PreCompile.h"
#include "LoginLevel.h"
#include "LoginUI.h"
#include "Enums.h"
#include "Player.h"
#include <GameEngineCore/GameEngineCameraActor.h>
#include <GameEngineBase/GameEngineInput.h>

LoginLevel::LoginLevel()
{
}

LoginLevel::~LoginLevel()
{
}


void LoginLevel::Start() {
	GameEngineDirectory Dir;
	Dir.MoveParentToExitsChildDirectory("ContentsResources");
	Dir.Move("ContentsResources");
	Dir.Move("Texture");
	Dir.Move("Title");

	std::vector<GameEngineFile> Shaders = Dir.GetAllFile();

	for (size_t i = 0; i < Shaders.size(); i++) {
		if (GameEngineTexture::Find(Shaders[i].GetFileName()) != nullptr)
			continue;

		GameEngineTexture::Load(Shaders[i].GetFullPath());

		if (Shaders[i].GetFileName() == "Draw1.png")
			GameEngineTexture::Cut(Shaders[i].GetFileName(), 2, 1);
	}

	LoginUI* Ptr = CreateActor<LoginUI>(GameObjectGroup::UI);

	Dir.MoveParentToExitsChildDirectory("ContentsResources");
	Dir.Move("ContentsResources");
	Dir.Move("Texture");
	Dir.Move("Loading");

	Shaders = Dir.GetAllFile();

	for (size_t i = 0; i < Shaders.size(); i++) {
		if (GameEngineTexture::Find(Shaders[i].GetFileName()) != nullptr)
			continue;

		GameEngineTexture::Load(Shaders[i].GetFullPath());
	}
}

void LoginLevel::LevelStartEvent()
{
}

void LoginLevel::Update(float _DeltaTime)
{

}

void LoginLevel::End()
{
}