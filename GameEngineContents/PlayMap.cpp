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

void PlayMap::OpenDoors() {
	for (auto iter = Doors.begin(); iter != Doors.end(); iter++) {
		if (!iter->second->GetRenderer()->IsUpdate())
			continue;
		iter->second->GetState().ChangeState("Open");
	}
}

void PlayMap::CloseDoors() {
	for (auto iter = Doors.begin(); iter != Doors.end(); iter++) {
		if (!iter->second->GetRenderer()->IsUpdate())
			continue;
		iter->second->GetState().ChangeState("Close");
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
		if (_Door->IsOpen() && Player::GetMainPlayer()->GetInvincible() == false) {
			_Door->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Player, CollisionType::CT_OBB2D, _Function);
		} else {
			_Door->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Player, CollisionType::CT_OBB2D);
		}

		_Door->GetCollision()->IsCollisionRigidBody(CollisionType::CT_OBB2D, OBJECTORDER::Monster, CollisionType::CT_OBB2D);
	}
}

void PlayMap::MapOn(int* _DoorOpen) {
	On();

	// 문이 아니라 벽일 경우
	for (int i = 0; i < Doors.size(); i++) {
		Door* _Door = Doors[i];
		GameRoomType RoomType = static_cast<GameRoomType>(_DoorOpen[i]);
		if (RoomType == GameRoomType::Wall) {
			_Door->SetRendererState(false);
			_Door->GetState().ChangeState("Wall");
		} else {
			_Door->SetRendererState(true);
			//_Door->SetType(RoomType);
		}
	}

	StateManager.ChangeState("Battle");
}

void PlayMap::MapOff() {
	StateManager.ChangeState("Off");
}

void PlayMap::Start() {
	float4 CutScale = GameEngineTexture::Find("01_basement.png")->GetCutScale(0);
	float4 WindowScale = GameEngineWindow::GetScale();
	float4 TexScale = { 424.f + 16.f, WindowScale.y / 2.f + 10.f };
	// 225
	float4 TileScale[4] = {
		{1.f, 1.f},
		{-1.f, 1.f},
		{1.f, -1.f},
		{-1.f, -1.f}
	};

	float4 TilePos[4] = {
		{0.f, 0.f},
		{TexScale.x, 0.f},
		{0.f, -TexScale.y},
		{TexScale.x, -TexScale.y}
	};

	PIVOTMODE TilePivot[4] = {
		PIVOTMODE::LEFTTOP,
		PIVOTMODE::RIGHTTOP,
		PIVOTMODE::LEFTBOT,
		PIVOTMODE::RIGHTBOT
	};
	// 234 156
	for (int i = 0; i < 4; i++) {
		auto Tile = CreateComponent<GameEngineTextureRenderer>();
		Tile->SetPivot(TilePivot[i]);
		Tile->GetTransform().SetLocalPosition({ TilePos[i].x, TilePos[i].y, 4000.f});
		Tile->SetTexture("01_basement.png", 0);
		Tile->GetTransform().SetLocalScale({ TileScale[i].x * TexScale.x , TileScale[i].y * TexScale.y });

		auto Floor = CreateComponent<GameEngineTextureRenderer>();
		Floor->SetPivot(TilePivot[i]);
		Floor->GetTransform().SetLocalPosition({ TilePos[i].x, TilePos[i].y, 4002.f });
		Floor->SetTexture("01_basement.png", 4);
		Floor->GetTransform().SetLocalScale({ TileScale[i].x * TexScale.x , TileScale[i].y * TexScale.y });
	}

	Renderer = CreateComponent<GameEngineTextureRenderer>();
	Renderer->GetTransform().SetWorldScale({ GameEngineWindow::GetScale().x, GameEngineWindow::GetScale().y, 1});
	Renderer->GetTransform().SetLocalPosition(float4(0, 0, 5000));
	Renderer->SetTexture("bgblack.png", 0);

	// 442 286
	auto Padding = CreateComponent<GameEngineTextureRenderer>();
	Padding->GetTransform().SetLocalPosition(float4(0, 0, 3000));
	Padding->SetTexture("shading.png", 0);
	Padding->GetTransform().SetLocalScale({ TexScale.x * 2.f , TexScale.y * 2.f });

	// 442 286
	auto Overlay = CreateComponent<GameEngineTextureRenderer>();
	Overlay->GetTransform().SetLocalPosition(float4(0, 0, -1000));
	std::string TexName = "1x1_overlay_" + to_string((rand() % 5) + 1) + ".png";
	Overlay->SetTexture(TexName);
	Overlay->GetPixelData().MulColor.a = 0.9f;
	Overlay->GetTransform().SetLocalScale({ TexScale.x * 2.f , TexScale.y * 2.f });

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

void PlayMap::Initialize(GameRoomType* _AdjacenceRoom) {
	if (RoomType == (int)GameRoomType::Start) {
		auto Help = CreateComponent<GameEngineTextureRenderer>();
		Help->GetTransform().SetLocalPosition(float4(0, 0, 3100));
		Help->SetTexture("controls.png");
		Help->ScaleToTexture();
	}

	RECT rc[16] = {
		{0, 265, 960, 50},
		{425, 0, 50, 520},
		{-425, 0, 50, 520},
		{0, -265, 960, 50},

		{-265, 212, 480, 50},
		{265, 212, 480, 50},
		{-265, -212, 480, 50},
		{265, -212, 480, 50},

		{368, 166, 50, 280},
		{368, -166, 50, 280},
		{-368, 166, 50, 280},
		{-368, -166, 50, 280},

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
		{0, -222, 12},
		{-378, 0, 12},
		{378, 0, 12},
		{0, 222, 12}
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
		if (Type != GameRoomType::Room &&
			Type != GameRoomType::Wall &&
			(_AdjacenceRoom[j] == GameRoomType::Room || _AdjacenceRoom[j] == GameRoomType::Start)) {
			NewDoor->SetType(Type);
		} else {
			NewDoor->SetType(_AdjacenceRoom[j]);
		}

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
	if (GameEngineInput::GetInst()->IsDown("Space")) {
		for (int i = 0; i < Doors.size(); i++) {
			Door* _Door = Doors[i];
			if (!_Door->GetRenderer()->IsUpdate())
				continue;

			if (_Door->IsOpen())
				_Door->GetState().ChangeState("Close");
			else {
				_Door->GetState().ChangeState("Open");
			}
		}
	}
}

int PlayMap::GetMapEnemyCount() {
	int Result = 0;
	for (auto _Actor : GetIncludeActors()) {
		if (_Actor->IsDeath())
			continue;

		if (_Actor->GetCollision() == nullptr)
			continue;

		if (_Actor->GetCollision()->GetOrder() == (int)OBJECTORDER::Monster)
			Result++;
	}
	return Result;
}

void PlayMap::BattleStart(const StateInfo& _Info) {
	TurnOnActors();
	if (GetMapEnemyCount() > 0) {
		CloseDoors();
		return;
	}

	OpenDoors();
}

void PlayMap::IdleStart(const StateInfo& _Info) {
	TurnOnActors();
	if (GetMapEnemyCount() > 0)
		return;

	OpenDoors();
}

void PlayMap::OffStart(const StateInfo& _Info) {
	TurnOffActors();
	Off();
}

void PlayMap::SetDoorState(std::string _Name) {
	for (int i = 0; i < Doors.size(); i++) {
		Door* _Door = Doors[i];
		if (!_Door->GetRenderer()->IsUpdate())
			continue;

		_Door->GetState().ChangeState(_Name);
	}
}

void PlayMap::BattleUpdate(float _DeltaTime, const StateInfo& _Info) {
}

void PlayMap::IdleUpdate(float _DeltaTime, const StateInfo& _Info) {
}

void PlayMap::OffUpdate(float _DeltaTime, const StateInfo& _Info) {
}
