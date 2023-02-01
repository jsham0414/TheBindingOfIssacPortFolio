#include "PreCompile.h"
#include "UIMaster.h"

UIMaster::UIMaster()
{
}

UIMaster::~UIMaster()
{
}

void UIMaster::Start()
{
	{
		GameEngineUIRenderer* Renderer = CreateComponent<GameEngineUIRenderer>();
		//Renderer->GetTransform().SetLocalScale({ 1280, 720, 1 });
		// Renderer->GetTransform().SetLocalPosition({ 300, 300, 1 });
		// Renderer->ChangeCamera(CAMERAORDER::MAINCAMERA);
	}
}

void UIMaster::Update(float _DeltaTime) {
}
