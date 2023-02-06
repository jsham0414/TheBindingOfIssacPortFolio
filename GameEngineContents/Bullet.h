#pragma once
#include <GameEngineCore/GameEngineActorInfo.h>

// Ό³Έν :
class Bullet : public GameEngineActor {
public:
	// constrcuter destructer
	Bullet();
	~Bullet();

	// delete Function
	Bullet(const Bullet& _Other) = delete;
	Bullet(Bullet&& _Other) noexcept = delete;
	Bullet& operator=(const Bullet& _Other) = delete;
	Bullet& operator=(Bullet&& _Other) noexcept = delete;

	void SetDirection(int _Direction);

	void CreateFrameAnimation();

	GameEngineTextureRenderer* GetRenderer() const { 
		return Renderer;
	}

	void SetAngle(float _Angle) {
		GetTransform().SetWorldRotation({ 0, 0, _Angle });
		Angle = _Angle;
	}

	void SetVelocity(float _Velocity) {
		Velocity = _Velocity;
	}

	void AddSpeed(float _Power) {
		Speed += _Power;
	}

	GameEngineTransform SubTransform;
	GameEngineStateManager StateManager;
protected:
	void Start() override;
	void Update(float _DeltaTime);
	bool MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void End() {}
private:
	void IdleStateStart(const StateInfo& _Info);
	void IdleStateUpdate(float _DeltaTime, const StateInfo& _Info);

	void DeathStateStart(const StateInfo& _Info);
	void DeathStateUpdate(float _DeltaTime, const StateInfo& _Info);

	float Angle;
	float Speed;
	int Direction;
	DWORD Duration;
	DWORD DeadTime;

	float Lean;
	float Gravity;
	float StartPosition;

	float lastY;
	float Power;

	float EndTime;
	float MaxHeight;
	float Height;
	float EndHeight;
	float Time;
	float MaxTime;
	float4 StartPos;
	float4 EndPos;

	float Velocity;

	float Distance;
	float Accumulate;
};
