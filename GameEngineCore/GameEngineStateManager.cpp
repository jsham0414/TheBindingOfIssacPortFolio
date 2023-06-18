#include "PreCompile.h"
#include "GameEngineStateManager.h"

GameEngineStateManager::GameEngineStateManager() 
{
}

GameEngineStateManager::~GameEngineStateManager() 
{
}

void GameEngineStateManager::Update(float _DeltaTime)
{
	if (CurState == nullptr)
		return;

	if (CurState->GetNameConstRef() == "")
		return;

	CurState->StateUpdate(_DeltaTime);
}