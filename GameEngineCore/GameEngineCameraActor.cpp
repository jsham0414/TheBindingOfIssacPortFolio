#include "PreCompile.h"
#include "GameEngineCameraActor.h"
#include <GameEngineBase/GameEngineInput.h>
#include "GameEngineContents/MapManager.h"

GameEngineCameraActor::GameEngineCameraActor()
	: CameraComponent(nullptr)
	, RotSpeed(180.0f)
	, Speed(500.0f)
	, CameraMoving(false) {
}

GameEngineCameraActor::~GameEngineCameraActor() {
}

void GameEngineCameraActor::Start() {
	CameraComponent = CreateComponent<GameEngineCamera>();

	if (false == GameEngineInput::GetInst()->IsKey("CamMoveForward")) {
		GameEngineInput::GetInst()->CreateKey("CamMoveForward", 'W');
		GameEngineInput::GetInst()->CreateKey("CamMoveBack", 'S');
		GameEngineInput::GetInst()->CreateKey("CamMoveUp", 'Q');
		GameEngineInput::GetInst()->CreateKey("CamMoveDown", 'E');
		GameEngineInput::GetInst()->CreateKey("CamMoveLeft", 'A');
		GameEngineInput::GetInst()->CreateKey("CamMoveRight", 'D');

		GameEngineInput::GetInst()->CreateKey("CamMoveBoost", VK_LSHIFT);
		GameEngineInput::GetInst()->CreateKey("CamRot", VK_RBUTTON);
	}


	StateManager.CreateStateMember("Idle"
		, std::bind(&GameEngineCameraActor::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&GameEngineCameraActor::IdleStart, this, std::placeholders::_1)
	);

	StateManager.CreateStateMember("Moving"
		, std::bind(&GameEngineCameraActor::MovingUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&GameEngineCameraActor::MovingStart, this, std::placeholders::_1)
	);

	StateManager.ChangeState("Idle");
}

void GameEngineCameraActor::Update(float _DeltaTime) {
	if (false == FreeCameraMode) {
		StateManager.Update(_DeltaTime);
		return;
	}

	float MoveSpeed = Speed;

	if (GameEngineInput::GetInst()->IsPress("CamMoveBoost")) {
		MoveSpeed *= 3.0f;
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveForward")) {
		GetTransform().SetWorldForwardMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveBack")) {
		GetTransform().SetWorldBackMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveUp")) {
		GetTransform().SetWorldUpMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveDown")) {
		GetTransform().SetWorldDownMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveLeft")) {
		GetTransform().SetWorldLeftMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamMoveRight")) {
		GetTransform().SetWorldRightMove(MoveSpeed, _DeltaTime);
	}

	if (GameEngineInput::GetInst()->IsPress("CamRot")) {
		float4 MouseDir = CameraComponent->GetMouseWorldDir();
		float4 RotMouseDir;
		RotMouseDir.x = -MouseDir.y;
		RotMouseDir.y = MouseDir.x;

		GetTransform().SetAddWorldRotation(RotMouseDir * RotSpeed);
	}
}

bool GameEngineCameraActor::CameraMove(float4 _DestPos, float _Time) {
	_DestPos += float4::BACK * 1000.f;

	if (GetTransform().GetWorldPosition().x == _DestPos.x &&
		GetTransform().GetWorldPosition().y == _DestPos.y)
		return false;

	PrevPos = GetTransformData().WorldPosition;
	DestPos = _DestPos;
	MoveTime = _Time;

	StateManager.ChangeState("Moving");
	return true;
}

void GameEngineCameraActor::End() {

}

void GameEngineCameraActor::MovingStart(const StateInfo& _Info) {
	CameraMoving = true;
	RoomCheck = false;
}

void GameEngineCameraActor::MovingUpdate(float _DeltaTime, const StateInfo& _Info) {
	float NowTime = _Info.StateTime / MoveTime;
	float ProgressTime = fmin(1.f, NowTime);
	float4 NewPos = float4::Lerp(PrevPos, DestPos, GameEngineMath::EaseOutQuint(ProgressTime));
	GetTransform().SetWorldPosition(NewPos);

	if (GetLevel()->CompareName("Play")) {
		if (ProgressTime > 0.5f && !RoomCheck) {
			RoomCheck = true;
			GetLevel()->GetMapManager()->CheckEnemies(true);
		}

		if (ProgressTime >= 1.f) {
			GetLevel()->GetMapManager()->TurnOffLastRoom();
			StateManager.ChangeState("Idle");
		}
	} else {
		if (ProgressTime >= 1.f) {
			StateManager.ChangeState("Idle");
		}
	}
}

void GameEngineCameraActor::IdleStart(const StateInfo& _Info) {
	CameraMoving = false;
}

void GameEngineCameraActor::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
}


void GameEngineCameraActor::FreeCameraModeOnOff() {
	FreeCameraMode = !FreeCameraMode;

	if (true == FreeCameraMode) {
		PrevMode = CameraComponent->GetProjectionMode();
		CameraComponent->SetProjectionMode(CAMERAPROJECTIONMODE::PersPective);
		OriginTrans.Copy(GetTransform());
	} else {
		CameraComponent->SetProjectionMode(PrevMode);
		GetTransform().Copy(OriginTrans);
	}
}