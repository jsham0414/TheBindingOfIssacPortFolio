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
		if (_Wall->GetOrder() != static_cast<int>(OBJECTORDER::Wall))
			continue;

		_Wall->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Player, CollisionType::CT_OBB2D);
		_Wall->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Monster, CollisionType::CT_OBB2D);
	}

	for (auto iter = Doors.begin(); iter != Doors.end(); iter++) {
		Door* _Door = iter->second;

		_Door->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Player, CollisionType::CT_OBB2D, _Function);
		_Door->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Monster, CollisionType::CT_OBB2D);
	}
}

void PlayMap::MapOn(int* _DoorOpen) {
	On();
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

	// 몬스터 남아 있을 때
	StateManager.CreateStateMember("Battle"
		, std::bind(&PlayMap::BattleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&PlayMap::BattleStart, this, std::placeholders::_1)
	);

	// 몬스터 다 잡고 문 열릴 때
	StateManager.CreateStateMember("Idle"
		, std::bind(&PlayMap::IdleUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&PlayMap::IdleStart, this, std::placeholders::_1)
	);

	// 몬스터 다 잡고 문 열릴 때
	StateManager.CreateStateMember("Off"
		, std::bind(&PlayMap::OffUpdate, this, std::placeholders::_1, std::placeholders::_2)
		, std::bind(&PlayMap::OffStart, this, std::placeholders::_1)
	);
	StateManager.ChangeState("Battle");

}

void PlayMap::Initialize() {
	RECT rc[16] = {
		{0, 265, 960, 50},
		{425, 0, 50, 520},
		{-425, 0, 50, 520},
		{0, -265, 960, 50},

		{-265, 206, 480, 50},
		{265, 206, 480, 50},
		{-265, -205, 480, 50},
		{265, -205, 480, 50},

		{365, 166, 50, 280},
		{365, -166, 50, 280},
		{-365, 166, 50, 280},
		{-365, -166, 50, 280},

		// 총알용 벽
		{0, 260, 960, 50},
		{380, 0, 50, 520},
		{-380, 0, 50, 520},
		{0, -220, 960, 50},
	};

	for (int i = 0; i < _countof(rc); i++) {
		Wall* NewWall;

		if (i < 12) {
			NewWall = GetLevel()->CreateActor<Wall>(OBJECTORDER::Wall);
		} else {
			NewWall = GetLevel()->CreateActor<Wall>(OBJECTORDER::AirWall);
		}

		float4 Color = float4::GREEN;
		Color.a = 0.2f;

		//NewWall->SetParent(this);
		//NewWall->SetOrder((int));
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

	// TODO : 방 문 만들기
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
