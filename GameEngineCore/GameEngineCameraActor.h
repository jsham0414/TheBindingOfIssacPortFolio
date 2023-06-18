#pragma once
#include "GameEngineActor.h"
#include "GameEngineCamera.h"
#include "GameEngineStateManager.h"
#include <thread>

// Ό³Έν :
class GameEngineLevel;
class GameEngineCameraActor : public GameEngineActor
{

public:
	// constrcuter destructer
	GameEngineCameraActor();
	~GameEngineCameraActor();

	// delete Function
	GameEngineCameraActor(const GameEngineCameraActor& _Other) = delete;
	GameEngineCameraActor(GameEngineCameraActor&& _Other) noexcept = delete;
	GameEngineCameraActor& operator=(const GameEngineCameraActor& _Other) = delete;
	GameEngineCameraActor& operator=(GameEngineCameraActor&& _Other) noexcept = delete;

	GameEngineCamera* GetCameraComponent()
	{
		return CameraComponent;
	}

	void FreeCameraModeOnOff();

	inline bool IsFreeCameraMode() 
	{
		return FreeCameraMode;
	}

	void Update(float _DeltaTime) override;

	bool CameraMove(float4 _DestPos, float _Time = 1.f);

	bool IsMoving() {
		return CameraMoving;
	}

	GameEngineStateManager StateManager;
protected:

	void Start() override;
	void End() override;

	void MovingStart(const StateInfo& _Info);
	void MovingUpdate(float _DeltaTime, const StateInfo& _Info);

	void IdleStart(const StateInfo& _Info);
	void IdleUpdate(float _DeltaTime, const StateInfo& _Info);

private:
	float MoveTime;

	float4 PrevPos, DestPos;

	bool FreeCameraMode;
	float Speed;
	float RotSpeed;
	bool CameraMoving;
	bool RoomCheck;

	GameEngineCamera* CameraComponent;
	GameEngineTransform OriginTrans;
	CAMERAPROJECTIONMODE PrevMode;
};

