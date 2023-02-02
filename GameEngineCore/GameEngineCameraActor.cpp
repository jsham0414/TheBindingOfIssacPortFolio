#include "PreCompile.h"
#include "GameEngineCameraActor.h"
#include <GameEngineBase/GameEngineInput.h>
#include "GameEngineContents/MapManager.h"

GameEngineCameraActor::GameEngineCameraActor()
	: CameraComponent(nullptr)
	, RotSpeed(180.0f)
	, Speed(500.0f)
{
}

GameEngineCameraActor::~GameEngineCameraActor() 
{
}

void GameEngineCameraActor::Start() 
{
	CameraComponent = CreateComponent<GameEngineCamera>();

	if (false == GameEngineInput::GetInst()->IsKey("CamMoveForward"))
	{
		GameEngineInput::GetInst()->CreateKey("CamMoveForward", 'W');
		GameEngineInput::GetInst()->CreateKey("CamMoveBack", 'S');
		GameEngineInput::GetInst()->CreateKey("CamMoveUp", 'Q');
		GameEngineInput::GetInst()->CreateKey("CamMoveDown", 'E');
		GameEngineInput::GetInst()->CreateKey("CamMoveLeft", 'A');
		GameEngineInput::GetInst()->CreateKey("CamMoveRight", 'D');

		GameEngineInput::GetInst()->CreateKey("CamMoveBoost", VK_LSHIFT);
		GameEngineInput::GetInst()->CreateKey("CamRot", VK_RBUTTON);
	}

}

void GameEngineCameraActor::Update(float _DeltaTime)
{
	if (false == FreeCameraMode)
	{
		return;
	}

	float MoveSpeed = Speed;

	if (GameEngineInput::GetInst()->IsPress("CamMoveBoost"))
	{
		MoveSpeed *= 3.0f;
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveForward"))
	{
		GetTransform().SetWorldForwardMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveBack"))
	{
		GetTransform().SetWorldBackMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveUp"))
	{
		GetTransform().SetWorldUpMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveDown"))
	{
		GetTransform().SetWorldDownMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveLeft"))
	{
		GetTransform().SetWorldLeftMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveRight"))
	{
		GetTransform().SetWorldRightMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamRot"))
	{
		float4 MouseDir = CameraComponent->GetMouseWorldDir();
		float4 RotMouseDir;
		RotMouseDir.x = -MouseDir.y;
		RotMouseDir.y = MouseDir.x;

		GetTransform().SetAddWorldRotation(RotMouseDir * RotSpeed);
	}
}

void GameEngineCameraActor::MapChange(float4 _DestPos) {
	auto Thread = std::thread(&GameEngineCameraActor::MapChangeLerp, this, _DestPos);
	Thread.detach();
}

void GameEngineCameraActor::MapChangeLerp(float4 _DestPos) {
	DWORD LastTime = GetTickCount64();
	float4 LastPos = GetTransform().GetWorldPosition();
	float fTime = 0.f;
	while (fTime < 1.f) {
		DWORD NowTime = GetTickCount64() - LastTime;
		float4 NewPos;
		fTime = fmin((float)NowTime / 500.f, 1.f);
		NewPos.x = std::lerp(LastPos.x, _DestPos.x, fTime);
		NewPos.y = std::lerp(LastPos.y, _DestPos.y, fTime);

		Mutex.lock();
		GetTransform().SetWorldPosition(NewPos);
		Mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	GetLevel()->GetMapManager()->TurnOffLastRoom();

	Player::GetMainPlayer()->GetCollision()->On();
}

void GameEngineCameraActor::End() 
{

}


void GameEngineCameraActor::FreeCameraModeOnOff()
{
	FreeCameraMode = !FreeCameraMode;

	if (true == FreeCameraMode)
	{
		PrevMode = CameraComponent->GetProjectionMode();
		CameraComponent->SetProjectionMode(CAMERAPROJECTIONMODE::PersPective);
		OriginTrans.Copy(GetTransform());
	}
	else 
	{
		CameraComponent->SetProjectionMode(PrevMode);
		GetTransform().Copy(OriginTrans);
	}
}