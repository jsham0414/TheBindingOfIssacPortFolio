#include "PreCompile.h"
#include "SlotMachine.h"

SlotMachine::SlotMachine() {
}

SlotMachine::~SlotMachine() {
}

void SlotMachine::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();

	Collision = CreateComponent<GameEngineCollision>();
	Collision->SetDebugSetting(CollisionType::CT_AABB2D, float4::RED);
	Collision->GetTransform().SetLocalPosition({-6.f, -30.f});
	Collision->GetTransform().SetLocalScale({ 60.f, 20.f });
	Collision->GetTransform().SetStatic(true);

	CreateFrameAnimation();
	Renderer->SetTexture("slot_001_machine.png", 0);

	StateManager.CreateStateMember("Idle"
		, std::bind(&SlotMachine::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&SlotMachine::IdleStart, this, std::placeholders::_1)
	);
	StateManager.CreateStateMember("Trigger"
		, std::bind(&SlotMachine::TriggerUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&SlotMachine::TriggerStart, this, std::placeholders::_1)
	);

	for (int j = 0; j < 3; j++) {
		auto c = CreateComponent<GameEngineTextureRenderer>();
		c->SetTexture("slot_001_machine_card.png", 0);
		c->ScaleToCutTexture();
		// - Renderer->GetTransformData().LocalScaling.x
		c->GetTransform().SetLocalPosition({ c->GetTransformData().LocalScaling.x * j - 25.f , c->GetTransformData().LocalScaling.y - 22.f });
		Card[j] = c;
	}

	TableNumber = float4::ZERO;

	for (int j = 0; j < 3; j++) {
		CardNumber[j] = rand() % 6 + 1;
		Card[j]->SetTexture("slot_001_machine_card.png", CardNumber[j]);
		Card[j]->ScaleToCutTexture();
	}
	Lock = 0;
	Spin = false;

	StateManager.ChangeState("Idle");
}

void SlotMachine::Update(float _DeltaTime) {
	StateManager.Update(_DeltaTime);
}

void SlotMachine::IdleStart(const StateInfo& _Info) {
	if (!_Info.PrevState.empty())
		Renderer->ChangeFrameAnimation("Idle");
	Renderer->ScaleToCutTexture();
}

void SlotMachine::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D, std::bind(&SlotMachine::PlayerCollision, this, std::placeholders::_1, std::placeholders::_2));
}

bool SlotMachine::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	StateManager.ChangeState("Trigger");

	return true;
}

void SlotMachine::TriggerStart(const StateInfo& _Info) {
	Renderer->ChangeFrameAnimation("Trigger");
	Renderer->ScaleToCutTexture();

	float4 Table[10] = {
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
	};

	TableNumber = Table[rand() % (_countof(Table))];

	for (int j = 0; j < 3; j++) {
		CardNumber[j] = rand() % 6;
		Card[j]->SetTexture("slot_001_machine_card.png", CardNumber[j] + 8);
		Card[j]->ScaleToCutTexture();
	}
	Lock = 0;
	Spin = false;
}

#define ULL unsigned long long

void SlotMachine::TriggerUpdate(float _DeltaTime, const StateInfo& _Info) {
	static ULL LastTime = GetTickCount64();

	Collision->IsCollisionRigidBody(CollisionType::CT_AABB2D, OBJECTORDER::Player, CollisionType::CT_AABB2D, nullptr);

	if (LastTime < GetTickCount64()) {
		LastTime = GetTickCount64() + 50;
		for (int j = Lock; j < 3; j++) {
			CardNumber[j] = (CardNumber[j] + 1) % 6;
			Card[j]->SetTexture("slot_001_machine_card.png", CardNumber[j] + 8);
		}
	}

	if ((Lock != 0 && _Info.StateTime > 1.f) || (Lock == 0 && _Info.StateTime > 2.f)) {
		StateManager.ResetStateTime();

		Card[Lock]->SetFrame((int)TableNumber[Lock]);
		Lock++;

		if (Lock >= 3) {
			StateManager.ChangeState("Idle");
			return;
		}
	}
}

void SlotMachine::SpinStart(const StateInfo& _Info) {
}

void SlotMachine::SpinUpdate(float _DeltaTime, const StateInfo& _Info) {
}


void SlotMachine::CreateFrameAnimation() {
	Renderer->CreateFrameAnimationCutTexture("Idle", FrameAnimation_DESC("slot_001_machine.png", {2, 1, 0}, 0.05f, false));
	Renderer->CreateFrameAnimationCutTexture("Trigger", FrameAnimation_DESC("slot_001_machine.png", 0, 2, 0.05f, false));
}