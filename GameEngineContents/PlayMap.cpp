#include "PreCompile.h"
#include "PlayMap.h"

PlayMap::PlayMap() {
	Width = 10;
	
	Height = 10;

	RoomCount = 0;
	MaxRoomCount = 15;
	MinRoomCount = 7;

	StartPos = 45;

	SpecialRoomCount = 3;
}

PlayMap::~PlayMap() {
}

void PlayMap::TurnOnActors() {
	for (int i = 0; i < Walls.size(); i++) {
		Walls[i]->On();
	}
	for (auto iter = Doors.begin(); iter != Doors.end(); iter++) {
		iter->second->On();
	}
	for (auto _Actors : IncludeActors) {
		_Actors->On();
	}
}

void PlayMap::TurnOffActors() {
	for (int i = 0; i < Walls.size(); i++) {
		Walls[i]->Off();
	}
	for (auto iter = Doors.begin(); iter != Doors.end(); iter++) {
		iter->second->Off();
	}
	for (auto _Actors : IncludeActors) {
		_Actors->Off();
	}
}

void PlayMap::UpdateCollision(function<bool(GameEngineCollision*, GameEngineCollision*)> _Function) {
	for (int i = 0; i < Walls.size(); i++) {
		Wall* _Wall = Walls[i];

		_Wall->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Player, CollisionType::CT_OBB2D, _Function);
	}

	for (auto iter = Doors.begin(); iter != Doors.end(); iter++) {
		Door* _Door = iter->second;

		_Door->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Player, CollisionType::CT_OBB2D, _Function);
	}
}

void PlayMap::MapOn(int* _DoorOpen) {
	On();
	GetLevel()->GetMainCameraActorTransform().SetWorldPosition(GetTransform().GetWorldPosition() + float4::BACK * 100.f);

	for (int i = 0; i < Doors.size(); i++) {
		Door* _Door = Doors[i];
		GameRoomType RoomType = static_cast<GameRoomType>(_DoorOpen[i]);
		if (RoomType != GameRoomType::Wall) {
			_Door->GetRenderer()->On();
			_Door->SetType(RoomType);
		} else {
			_Door->GetRenderer()->Off();
			_Door->GetState().ChangeState("Battle");
		}
	}

	StateManager.ChangeState("Battle");
}

void PlayMap::MapOff() {
	StateManager.ChangeState("Off");
}

void PlayMap::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->GetTransform().SetWorldScale({ 960.f, 540.f, 1 });
	Renderer->GetTransform().SetWorldPosition(float4(0, 0, 100));
	Renderer->SetTexture("room_001.png");

	// ���� ���� ���� ��
	StateManager.CreateStateMember("Battle"
		, std::bind(&PlayMap::BattleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&PlayMap::BattleStart, this, std::placeholders::_1)
	);

	// ���� �� ��� �� ���� ��
	StateManager.CreateStateMember("Idle"
		, std::bind(&PlayMap::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&PlayMap::IdleStart, this, std::placeholders::_1)
	);

	// ���� �� ��� �� ���� ��
	StateManager.CreateStateMember("Off"
		, std::bind(&PlayMap::OffUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&PlayMap::OffStart, this, std::placeholders::_1)
	);
	StateManager.ChangeState("Battle");

}

void PlayMap::Initialize() {
	RECT rc[12] = {
		{0, 390, 960, 300},
		{550, 0, 300, 520},
		{-550, 0, 300, 520},
		{0, -390, 960, 300},
		{-500, 330, 960, 300},
		{500, 330, 960, 300},
		{-500, -330, 960, 300},
		{500, -330, 960, 300},
		{490, 280, 300, 520},
		{490, -280, 300, 520},
		{-490, 280, 300, 520},
		{-490, -280, 300, 520},
	};

	for (int i = 0; i < _countof(rc); i++) {
		Wall* NewWall = GetLevel()->CreateActor<Wall>();
		float4 Color = float4::GREEN;
		Color.a = 0.2f;

		//NewWall->SetParent(this);
		NewWall->SetOrder((int)OBJECTORDER::Wall);
		NewWall->GetCollision()->SetDebugSetting(CollisionType::CT_AABB, Color);

		float4 NewPos = GetTransform().GetWorldPosition();
		NewPos.x += (float)rc[i].left;
		NewPos.y += (float)rc[i].top;
		NewWall->GetTransform().SetWorldPosition(NewPos);
		NewWall->GetCollision()->GetTransform().SetWorldScale({ (float)(rc[i].right), (float)(rc[i].bottom) });
		NewWall->Off();

		Walls.push_back(NewWall);
	}

	float4 DoorPos[4] = {
		{0, -218, 12},
		{-378, 0, 12},
		{378, 0, 12},
		{0, 218, 12}
	};

	float4 DoorRotation[4] = {
		{0, 0.f, 180.f},
		{0, 0.f, 90.f},
		{0, 0.f, 270.f},
		{0, 0.f, 0.f},
	};

	// TODO : �� �� �����
	for (int j = 0; j < 4; j++) {
		Door* NewDoor = GetLevel()->CreateActor<Door>();

		NewDoor->GetTransform().SetLocalRotation(DoorRotation[j]);
		auto NewPos = GetTransform().GetWorldPosition() + DoorPos[j];
		NewDoor->GetTransform().SetWorldPosition(NewPos);
		NewDoor->Initialize();
		NewDoor->GetRenderer()->Off();
		NewDoor->Off();
		Doors[j] = NewDoor;
	}
}

void PlayMap::Update(float _DeltaTime) {
}

void PlayMap::BattleStart(const StateInfo& _Info) {
	TurnOnActors();
}

void PlayMap::IdleStart(const StateInfo& _Info) {
	TurnOnActors();
}

void PlayMap::OffStart(const StateInfo& _Info) {
	TurnOffActors();
	Off();
}

void PlayMap::BattleUpdate(float _DeltaTime, const StateInfo& _Info) {
}

void PlayMap::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
}

void PlayMap::OffUpdate(float _DeltaTime, const StateInfo& _Info) {
}