#pragma once
#include <GameEngineBase/GameEngineTransform.h>
#include "GameEngineTransformComponent.h"

// 설명 :
class GameEngineCollisionFunctionInit;
class GameEngineCollision : public GameEngineTransformComponent
{
	friend GameEngineCollisionFunctionInit;

private:
	static bool (*CollisionFunction[static_cast<int>(CollisionType::CT_MAX)][static_cast<int>(CollisionType::CT_MAX)])(const GameEngineTransform& _Left, const GameEngineTransform& _Right);

public:
	// constrcuter destructer
	GameEngineCollision();
	~GameEngineCollision();

	// delete Function
	GameEngineCollision(const GameEngineCollision& _Other) = delete;
	GameEngineCollision(GameEngineCollision&& _Other) noexcept = delete;
	GameEngineCollision& operator=(const GameEngineCollision& _Other) = delete;
	GameEngineCollision& operator=(GameEngineCollision&& _Other) noexcept = delete;

	// Collision Data를 Level에 전달해줘야 하기 때문에 GameEngineCollision은 SetOrder를 쓰지 않는다
	template<typename EnumType>
	void ChangeOrder(EnumType _Order)
	{
		ChangeOrder(static_cast<int>(_Order));
	}

	void ChangeOrder(int _Order);

	//bool

	template<typename EnumType>
	bool IsCollision(CollisionType _ThisType, EnumType _GroupOrder
		, CollisionType _OtherCollision
		, std::function<bool(GameEngineCollision* _This, GameEngineCollision* _Other)> _Function = nullptr) 
	{
		return IsCollision(_ThisType, static_cast<int>(_GroupOrder), _OtherCollision, _Function);
	}

	// 충돌을 했냐 안했냐?
	// 넣어줘야할 함수의 리턴하는 bool값이 true라는 건 이제 더이상 충돌체크를 하지 말라는 end의 true이다.
	bool IsCollision(CollisionType _ThisType, int _GroupOrder
		, CollisionType _OtherType
		, std::function<bool(GameEngineCollision* _This, GameEngineCollision* _Other)> _Function = nullptr);

	void Rotate(float* nx, float* ny, float tx, float ty, float cx, float cy, float q);

	template<typename EnumType>
	bool IsCollisionRigidBody(CollisionType _ThisType, EnumType _GroupOrder
		, CollisionType _OtherCollision
		, std::function<bool(GameEngineCollision* _This, GameEngineCollision* _Other)> _Function = nullptr) {
		return IsCollisionRigidBody(_ThisType, static_cast<int>(_GroupOrder), _OtherCollision, _Function);
	}
	// 물리적 충돌
	bool IsCollisionRigidBody(CollisionType _ThisType, int _GroupOrder
		, CollisionType _OtherType
		, std::function<bool(GameEngineCollision* _This, GameEngineCollision* _Other)> _Function = nullptr);

	void SetDebugSetting(CollisionType _DebugType, float4 _Color) 
	{
		DebugType = _DebugType;
		Color = _Color;
	}

	virtual void DebugRender();

	void SetUIDebugCamera();

	void SetDebugCamera(CAMERAORDER _Order)
	{
		DebugCameraOrder = _Order;
	}

	float GetMass() {
		return Mass;
	}

	void SetMass(float _Mass) {
		Mass = _Mass;
	}

	float GetPower() {
		return Power;
	}

	void SetPower(float _Power) {
		Power = _Power;
	}

protected:

private:
	CAMERAORDER DebugCameraOrder;

	CollisionType DebugType;
	float4 Color;
	float Mass, Power;

	void Start() override;
};

