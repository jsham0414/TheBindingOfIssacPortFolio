#include "PreCompile.h"
#include "GameEngineLevel.h"
#include "GameEngineActor.h"
#include "GameEngineRenderer.h"
#include "GameEngineCamera.h"
#include "GameEngineRenderTarget.h"
#include "GameEngineCameraActor.h"
#include "GameEngineCollision.h"
#include "GameEngineGUI.h"
#include "GameEngineCoreDebug.h"
#include "GEngine.h"
#include <GameEngineContents/GlobalContentsValue.h>
#include "GameEngineContents/MapManager.h"

GameEngineLevel::GameEngineLevel() 
{
	MapManagerInstance = nullptr;

	Cameras.resize(static_cast<unsigned int>(CAMERAORDER::UICAMERA));

	{
		GameEngineCameraActor* CameraActor = CreateActor<GameEngineCameraActor>();
		CameraActor->GetTransform().SetLocalPosition({ 0.0f, 0.0f, -100.0f });
		CameraActor->GetCameraComponent()->SetProjectionMode(CAMERAPROJECTIONMODE::Orthographic);
		CameraActor->GetCameraComponent()->SetCameraOrder(CAMERAORDER::MAINCAMERA);
	}

	{
		GameEngineCameraActor* CameraActor = CreateActor<GameEngineCameraActor>();
		CameraActor->GetTransform().SetLocalPosition({ 0.0f, 0.0f, -100.0f });
		CameraActor->GetCameraComponent()->SetProjectionMode(CAMERAPROJECTIONMODE::Orthographic);
		CameraActor->GetCameraComponent()->SetCameraOrder(CAMERAORDER::UICAMERA);
	}
}

GameEngineLevel::~GameEngineLevel() 
{
	for (const std::pair<int, std::list<GameEngineActor*>>& Group : AllActors)
	{
		for (GameEngineActor* Actor : Group.second)
		{
			if (nullptr == Actor)
			{
				continue;
			}

			Actor->ReleaseHierarchy();
		}
	}
}


void GameEngineLevel::ActorUpdate(float _DeltaTime) {
	for (const std::pair<int, std::list<GameEngineActor*>>& Group : AllActors) {
		//AllActors
		// float ScaleTime = GameEngineTime::GetInst()->GetDeltaTime(Group.first);
		if (Group.first == (int)OBJECTORDER::TileMapObject)
			continue;

		for (GameEngineActor* const Actor : Group.second) {
			if (false == Actor->IsUpdate()) {
				continue;
			}
			if (Actor->IsUpdateRef())
				Actor->On();

			Actor->AllUpdate(_DeltaTime);

		}
	}
}


void GameEngineLevel::ActorTransformUpdate(float _DeltaTime) {
	for (const std::pair<int, std::list<GameEngineActor*>>& Group : AllActors) {
		//AllActors
		// float ScaleTime = GameEngineTime::GetInst()->GetDeltaTime(Group.first);
		if (Group.first == (int)OBJECTORDER::TileMapObject)
			continue;

		for (GameEngineActor* const Actor : Group.second) {
			if (false == Actor->IsUpdate()) {
				continue;
			}
			Actor->GetTransform().Update(_DeltaTime);
		}
	}
}

void GameEngineLevel::ActorLevelStartEvent() 
{
	for (const std::pair<int, std::list<GameEngineActor*>>& Group : AllActors)
	{
		float ScaleTime = GameEngineTime::GetInst()->GetDeltaTime(Group.first);
		for (GameEngineActor* const Actor : Group.second)
		{
			if (false == Actor->IsUpdate())
			{
				continue;
			}
			// ???? ?????? ?????? ?????????
			Actor->AllLevelStartEvent();
		}
	}
}

void GameEngineLevel::ActorLevelEndEvent()
{
	for (const std::pair<int, std::list<GameEngineActor*>>& Group : AllActors)
	{
		float ScaleTime = GameEngineTime::GetInst()->GetDeltaTime(Group.first);
		for (GameEngineActor* const Actor : Group.second)
		{
			if (false == Actor->IsUpdate())
			{
				continue;
			}
			Actor->AllLevelEndEvent();
		}
	}
}

void GameEngineLevel::PushRenderer(GameEngineRenderer* _Renderer, int _CameraOrder)
{
	// ???? ?????? ???? ???????? ??????

	GameEngineCamera* PrevCamera = Cameras[static_cast<UINT>(_Renderer->CameraOrder)];

	PrevCamera->AllRenderer_[_Renderer->GetRenderingOrder()].remove(_Renderer);

	_Renderer->CameraOrder = static_cast<CAMERAORDER>(_CameraOrder);

	GameEngineCamera* NextCamera = Cameras[_CameraOrder];
	// ???? ???????? ?????????? ????????.
	Cameras[_CameraOrder]->PushRenderer(_Renderer);
	_Renderer->Camera = NextCamera;
}

void GameEngineLevel::PushCamera(GameEngineCamera* _Camera, int _CameraOrder)
{
	Cameras.resize(_CameraOrder + 1);

	Cameras[_CameraOrder] = _Camera;
}

GameEngineTransform& GameEngineLevel::GetMainCameraActorTransform()
{
	return Cameras[static_cast<int>(CAMERAORDER::MAINCAMERA)]->GetActor()->GetTransform();
}


GameEngineCameraActor* GameEngineLevel::GetMainCameraActor()
{
	return Cameras[static_cast<int>(CAMERAORDER::MAINCAMERA)]->GetActor<GameEngineCameraActor>();
}

GameEngineTransform& GameEngineLevel::GetUICameraActorTransform()
{
	return Cameras[static_cast<int>(CAMERAORDER::UICAMERA)]->GetActor()->GetTransform();
}


GameEngineCameraActor* GameEngineLevel::GetUICameraActor()
{
	return Cameras[static_cast<int>(CAMERAORDER::UICAMERA)]->GetActor<GameEngineCameraActor>();
}



void GameEngineLevel::Render(float _DelataTime)
{
	{
		if (true == GEngine::IsCollisionDebug())
		{
			std::map<int, std::list<GameEngineCollision*>>::iterator StartGroupIter = AllCollisions.begin();
			std::map<int, std::list<GameEngineCollision*>>::iterator EndGroupIter = AllCollisions.end();
			for (; StartGroupIter != EndGroupIter; ++StartGroupIter)
			{
				std::list<GameEngineCollision*>& Group = StartGroupIter->second;
				std::list<GameEngineCollision*>::iterator GroupStart = Group.begin();
				std::list<GameEngineCollision*>::iterator GroupEnd = Group.end();
				for (; GroupStart != GroupEnd; ++GroupStart)
				{
					if (true == (*GroupStart)->IsUpdate())
					{
						(*GroupStart)->DebugRender();
					}
				}
			}
		}
	}

	GameEngineDevice::RenderStart();

	// ?? ???????? ???????? ???? ??????.
	for (size_t i = 0; i < Cameras.size(); i++)
	{
		if (nullptr == Cameras[i])
		{
			continue;
		}

		Cameras[i]->Render(_DelataTime);
	}

	for (size_t i = 0; i < Cameras.size(); i++)
	{
		if (nullptr == Cameras[i])
		{
			continue;
		}

		Cameras[i]->GetCameraRenderTarget()->EffectProcess();
	}

	for (size_t i = 0; i < Cameras.size(); i++)
	{
		if (nullptr == Cameras[i])
		{
			continue;
		}

		GameEngineDevice::GetBackBuffer()->Merge(Cameras[i]->CameraRenderTarget, 0);
	}

	GameEngineDevice::GetBackBuffer()->EffectProcess();


	// ?????? ???????? ??????.
	GameEngineDebug::Debug3DRender();

	GameEngineGUI::GUIRender(this, _DelataTime);

	GameEngineDevice::RenderEnd();
}

void GameEngineLevel::PushActor(GameEngineActor* _Actor, int _ObjectGroupIndex)
{
	if (nullptr != _Actor->GetParent())
	{
		MsgBoxAssert("?????? ???????? ?????????? ?????? ???? ????????.");
	}

	std::list<GameEngineActor*>& Group = AllActors[_ObjectGroupIndex];

	Group.push_back(_Actor);
}


void GameEngineLevel::Release(float _DelataTime)
{
	for (GameEngineUpdateObject* Object : DeleteObject) {
		Object->ReleaseHierarchy();
	}

	DeleteObject.clear();

	for (size_t i = 0; i < Cameras.size(); i++) {
		if (nullptr == Cameras[i]) {
			continue;
		}

		Cameras[i]->Release(_DelataTime);
	}
	{
		std::map<int, std::list<GameEngineCollision*>>::iterator StartGroupIter = AllCollisions.begin();
		std::map<int, std::list<GameEngineCollision*>>::iterator EndGroupIter = AllCollisions.end();

		for (; StartGroupIter != EndGroupIter; ++StartGroupIter)
		{
			std::list<GameEngineCollision*>& Group = StartGroupIter->second;

			std::list<GameEngineCollision*>::iterator GroupStart = Group.begin();
			std::list<GameEngineCollision*>::iterator GroupEnd = Group.end();
			for (; GroupStart != GroupEnd; )
			{
				if (true == (*GroupStart)->IsDeath())
				{
					GroupStart = Group.erase(GroupStart);
				}
				else
				{
					++GroupStart;
				}
			}
		}
	}
	std::map<int, std::list<GameEngineActor*>>::iterator StartGroupIter = AllActors.begin();
	std::map<int, std::list<GameEngineActor*>>::iterator EndGroupIter = AllActors.end();

	for (; StartGroupIter != EndGroupIter; ++StartGroupIter)
	{
		std::list<GameEngineActor*>& Group = StartGroupIter->second;

		std::list<GameEngineActor*>::iterator GroupStart = Group.begin();
		std::list<GameEngineActor*>::iterator GroupEnd = Group.end();

		for (; GroupStart != GroupEnd; )
		{
			(*GroupStart)->ReleaseObject(DeleteObject);

			if (true == (*GroupStart)->IsDeath())
			{
				GroupStart = Group.erase(GroupStart);
			}
			else 
			{
				++GroupStart;
			}
			
		}
	}

}

void GameEngineLevel::LevelUpdate(float _DeltaTime)
{
	AddAccTime(_DeltaTime);
	Update(_DeltaTime);
	ActorUpdate(_DeltaTime);
	ActorTransformUpdate(_DeltaTime);
	Render(_DeltaTime);
	Release(_DeltaTime);
}

// ?????? ???????? ????
// ?????????? ???????? ?????? ?????? ?????? ??????.
void GameEngineLevel::RemoveActor(GameEngineActor* _Actor)
{
	if (AllActors.end() == AllActors.find(_Actor->GetOrder()))
	{
		MsgBoxAssert("???????? ?????? ???????? ???????? ????????.");
	}

	AllActors[_Actor->GetOrder()].remove(_Actor);
}

void GameEngineLevel::PushCollision(GameEngineCollision* _Collision, int _Order)
{
	// ?????? ?????? ???????? ???????? ????????.
	AllCollisions[_Collision->GetOrder()].remove(_Collision);

	// ???? ?????? ??????.
	_Collision->SetOrder(_Order);

	// ?????? ?????? ????????.
	AllCollisions[_Collision->GetOrder()].push_back(_Collision);
}

void GameEngineLevel::OverChildMove(GameEngineLevel* _NextLevel)
{
	if (this == _NextLevel)
	{
		return;
	}

	// ?????? ????
	
	// ?????? ????
	// _NextLevel
	{
		std::map<int, std::list<GameEngineActor*>>::iterator StartGroupIter = AllActors.begin();
		std::map<int, std::list<GameEngineActor*>>::iterator EndGroupIter = AllActors.end();

		std::list<GameEngineActor*> OverList;

		for (; StartGroupIter != EndGroupIter; ++StartGroupIter)
		{
			std::list<GameEngineActor*>& Group = StartGroupIter->second;

			std::list<GameEngineActor*>::iterator GroupStart = Group.begin();
			std::list<GameEngineActor*>::iterator GroupEnd = Group.end();
			for (; GroupStart != GroupEnd; )
			{
				if (true == (*GroupStart)->IsLevelOver)
				{
					// ?????????? ????????
					OverList.push_back((*GroupStart));
					GroupStart = Group.erase(GroupStart);
				}
				else
				{
					++GroupStart;
				}

			}
		}

		// ?????????? ??????
		for (GameEngineActor* OverActor : OverList)
		{
			// ???????? ??????????
			OverActor->SetLevel(_NextLevel);
			_NextLevel->AllActors[OverActor->GetOrder()].push_back(OverActor);
		}
	}

	for (size_t i = 0; i < Cameras.size(); i++)
	{
		if (nullptr == Cameras[i])
		{
			continue;
		}

		Cameras[i]->OverRenderer(_NextLevel->Cameras[i]);
	}

	{
		std::map<int, std::list<GameEngineCollision*>>::iterator StartGroupIter = AllCollisions.begin();
		std::map<int, std::list<GameEngineCollision*>>::iterator EndGroupIter = AllCollisions.end();

		std::list<GameEngineCollision*> OverList;

		for (; StartGroupIter != EndGroupIter; ++StartGroupIter)
		{
			std::list<GameEngineCollision*>& Group = StartGroupIter->second;

			std::list<GameEngineCollision*>::iterator GroupStart = Group.begin();
			std::list<GameEngineCollision*>::iterator GroupEnd = Group.end();
			for (; GroupStart != GroupEnd; )
			{
				if (true == (*GroupStart)->GetRoot<GameEngineActor>()->IsLevelOver)
				{
					// ?????????? ????????
					OverList.push_back((*GroupStart));
					GroupStart = Group.erase(GroupStart);
				}
				else
				{
					++GroupStart;
				}

			}
		}

		// ?????????? ??????
		for (GameEngineCollision* OverActor : OverList)
		{
			_NextLevel->AllCollisions[OverActor->GetOrder()].push_back(OverActor);
		}
	}
}

void GameEngineLevel::AllClear() 
{
	{
		std::map<int, std::list<GameEngineActor*>>::iterator StartGroupIter = AllActors.begin();
		std::map<int, std::list<GameEngineActor*>>::iterator EndGroupIter = AllActors.end();

		std::list<GameEngineActor*> OverList;

		for (; StartGroupIter != EndGroupIter; ++StartGroupIter)
		{
			std::list<GameEngineActor*>& Group = StartGroupIter->second;
			std::list<GameEngineActor*>::iterator GroupStart = Group.begin();
			std::list<GameEngineActor*>::iterator GroupEnd = Group.end();
			for (; GroupStart != GroupEnd; ++GroupStart)
			{
				delete *GroupStart;
			}
		}
	}

	AllActors.clear();

	Cameras.clear();

	AllCollisions.clear();
}

