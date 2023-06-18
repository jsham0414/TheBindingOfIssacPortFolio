#include "PreCompile.h"
#include "LoadingLevel.h"

LoadingLevel::LoadingLevel() {
	SetName("Loading");
}

LoadingLevel::~LoadingLevel() {
}

void LoadingLevel::Start() {
	LoadTexture();
}

void LoadingLevel::Update(float _DeltaTime) {
}

void LoadingLevel::End() {
}

void LoadingLevel::LevelStartEvent() {
}

bool LoadingLevel::LoadTexture() {
	GameEngineDirectory Dir;
	Dir.MoveParentToExitsChildDirectory("ContentsResources");
	Dir.Move("ContentsResources");
	Dir.Move("Texture");
	Dir.Move("Loading");

	std::vector<GameEngineFile> Shaders = Dir.GetAllFile();

	for (size_t i = 0; i < Shaders.size(); i++) {
		if (GameEngineTexture::Find(Shaders[i].GetFileName()) != nullptr)
			continue;

		GameEngineTexture::Load(Shaders[i].GetFullPath());
	}

	return true;
}