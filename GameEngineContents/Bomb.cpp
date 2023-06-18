#include "PreCompile.h"
#include "Bomb.h"

Bomb::Bomb() : DelayTime(2000) {
}

Bomb::~Bomb() {
}

void Bomb::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Collision = CreateComponent<GameEngineCollision>();
	Collision->ChangeOrder(OBJECTORDER::Bomb);
	Collision->GetTransform().SetLocalPosition({ 0.f, 20.f });
	Collision->GetTransform().SetLocalScale({ 30.0f, 30.0f, 30.0f });

	StateManager.CreateStateMember("Idle",
		std::bind(&Bomb::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Bomb::IdleStart, this, std::placeholders::_1));

	StateManager.CreateStateMember("Dead",
		std::bind(&Bomb::DeadUpdate, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Bomb::DeadStart, this, std::placeholders::_1));

	CreateFrameAnimation();
	Renderer->ChangeFrameAnimation("0");
	Renderer->ScaleToCutTexture();

	StateManager.ChangeState("Idle");

	StartTime = GetTickCount64();

	Collision->SetMass(3);

	GetTransform().Accel = 0.1f;
}

void Bomb::Update(float _DeltaTime) {
	StateManager.Update(_DeltaTime);
}

void Bomb::CreateFrameAnimation() {
	Renderer->CreateFrameAnimationCutTexture("0", FrameAnimation_DESC("pickup_016_bomb.png", 0, 0, 0.3f));
	Renderer->CreateFrameAnimationCutTexture("Explosion", FrameAnimation_DESC("effect_029_explosion.png", 0, 11, 0.02f, false));
}

void Bomb::BombDelay(DWORD _Time) {
	std::this_thread::sleep_for(std::chrono::milliseconds(_Time));
	TerminateThread(DelayThread.native_handle(), NULL);
}

void Bomb::ColorChange(DWORD _Time) {
	auto& _PixelData = Renderer->GetPixelData();
	DWORD _LastTime = GetTickCount64();

	float4 _Color[4] = { { 1.1f, 1.1f, 1.1f, 1.f }, float4::RED, float4::YELLOW, float4::WHITE };
	int _Index = 0;
	while (_LastTime + _Time > GetTickCount64()) {
		DWORD CurTime = _LastTime + _Time - GetTickCount64();

		Mutex.lock();
		_PixelData.MulColor = _Color[_Index % _countof(_Color)];
		_Index++;
		Mutex.unlock();

		int _Wait = 100;
		if (CurTime < 1500) _Wait *= 0.5f;
		if (CurTime < 500) _Wait *= 0.5f;

		std::this_thread::sleep_for(std::chrono::milliseconds(_Wait));
	}
}

void Bomb::IdleStart(const StateInfo& _Info) {
	Collision->SetOrder((int)OBJECTORDER::Bomb);
	Renderer->SetPivotToVector({ 0.f, 0.3f, 0.f, 0.f });

	LastTime = GetTickCount64();
	ExplosionTime = GetTickCount64();
	Index = 0;
	DelayThread = std::thread(&Bomb::ColorChange, this, DelayTime);
	DelayThread.detach();
	//std::thread(&Bomb::BombDelay, this, DelayTime).detach();

	CollisionTrigger = true;
	LastVelocity = { 0.f };
}

bool Bomb::ObjectCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	CollisionTrigger = true;
	return true;
}

bool Bomb::DamageCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	if (_Other->GetOrder() == (int)OBJECTORDER::Monster) {
		auto Infos = static_cast<Monster*>(_Other->GetActor());
		Infos->Damage(2);
	} else if (_Other->GetOrder() == (int)OBJECTORDER::Player) {
		Player* _Player = static_cast<Player*>(_Other->GetActor());
		_Player->Assault(2);
	}

	return true;
}

void Bomb::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (ExplosionTime + DelayTime < GetTickCount64()) {
		StateManager.ChangeState("Dead");
		return;
	}

	if (CollisionTrigger != true) {
		Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D);
	} else {
		CollisionTrigger = false;
		Collision->IsCollision(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D,
			std::bind(&Bomb::ObjectCollision, this, std::placeholders::_1, std::placeholders::_2)
		);
	}
	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Wall, CollisionType::CT_AABB2D);
	Collision->IsCollisionRigidBody(CollisionType::CT_SPHERE2D, OBJECTORDER::Bomb, CollisionType::CT_SPHERE2D);

	// 더 잘 밀리게
	float4 Velocity = (LastVelocity * 0.9f + GetTransform().Accel * 0.9f);
	GetTransform().SetWorldMove(Velocity * _DeltaTime);

	LastVelocity = Velocity;

	float4 Pump[4] = { {1.f, 1.f}, {0.9f, 1.1f}, {1.f, 1.f}, {1.2f, 0.8f} };
	if (LastTime < GetTickCount64()) {
		DWORD CurTime = StartTime + DelayTime - GetTickCount64();

		float Wait = 100;
		if (CurTime < 1500) {
			GetTransform().SetLocalScale(Pump[Index++ % _countof(Pump)]);
			Wait *= 0.25f;
		}
		if (CurTime < 300) Wait *= 0.5f;
		LastTime = GetTickCount64() + Wait;
	}
}

void Bomb::DeadStart(const StateInfo& _Info) {
	GetTransform().StopPhysics();
	Renderer->GetPixelData().MulColor = float4::WHITE;
	Renderer->GetTransform().SetLocalPosition({ 0.f, 15.f });
	GetTransform().SetLocalScale({ 1.f, 1.f });
	Renderer->ChangeFrameAnimation("Explosion");
	Renderer->ScaleToCutTexture();

	Collision->GetTransform().SetLocalScale({ 150.f, 150.f, 1.f });

	Collision->IsCollision(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D,
		std::bind(&Bomb::DamageCollision, this, std::placeholders::_1, std::placeholders::_2)
	);
	Collision->IsCollision(CollisionType::CT_AABB2D, OBJECTORDER::Monster, CollisionType::CT_AABB2D,
		std::bind(&Bomb::DamageCollision, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void Bomb::DeadUpdate(float _DeltaTime, const StateInfo& _Info) {
	if (Renderer->CurAnimationEnd()) {
		Death();
	}
}
