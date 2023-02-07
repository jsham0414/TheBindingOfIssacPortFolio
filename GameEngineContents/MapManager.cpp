#include "PreCompile.h"
#include "MapManager.h"
#include <GameEngineContents/Enums.h>
#include <fstream>
#include <GameEngineContents/Door.h>
#include <GameEngineContents/Wall.h>

const int MapManager::Move[4] = {
	10, -1, 1, -10
};

MapManager::MapManager() {
	Width = 10;
	Height = 10;

	RoomCount = 0;
	MaxRoomCount = 15;
	MinRoomCount = 7;

	StartPos = 45;

	SpecialRoomCount = 3;
	LastRoomIndex = -1;
}

MapManager::~MapManager() {
}

void MapManager::Start() {
	//RandomMapGenerate();
	CurRoomIndex = StartPos;

	//for (auto it = MapFormData.begin(); it != MapFormData.end();) {
	//	auto Items = (*it).second;
	//	for_each(Items.begin(), Items.end(), CONTAINER_DELETE());
	//}
}

PlayMap* MapManager::GetCurrentMap() {
	Mutex.lock();
	if (PlayMaps.empty() || PlayMaps.at(CurRoomIndex) == nullptr) {
		Mutex.unlock();
		return nullptr;
	}
	Mutex.unlock();

	return PlayMaps[CurRoomIndex];
}

PlayMap* MapManager::GetMap(int _Index) {
	Mutex.lock();
	if (PlayMaps.empty() || PlayMaps.at(_Index) == nullptr) {
		Mutex.unlock();
		return nullptr;
	}
	Mutex.unlock();

	return PlayMaps[_Index];
}

int MapManager::GetCurMapEnemyCount() {
	int Result = 0;
	PlayMap* CurMap = GetCurrentMap();
	if (CurMap == nullptr)
		return -1;

	for (auto _Actor : CurMap->GetIncludeActors()) {
		if (_Actor->GetCollision()->GetOrder() == (int)OBJECTORDER::Monster)
			Result++;
	}
	return Result;
}

void MapManager::Update(float _DeltaTime) {
	PlayMap* CurMap = GetCurrentMap();
	if (CurMap == nullptr)
		return;

	if (GetCurMapEnemyCount() <= 0) {
		auto& CurrentState = CurMap->GetState();
		if (CurrentState.GetCurStateName() == "Battle") {
			CurrentState.ChangeState("Idle");
		}
	}

	auto Func = std::bind(&MapManager::PlayerCollision, this, std::placeholders::_1, std::placeholders::_2);
	CurMap->UpdateCollision(Func);
}

bool MapManager::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	int Index = -1;

	LastRoomIndex = CurRoomIndex;
	PlayMap* CurMap = GetCurrentMap();
	if (CurMap == nullptr)
		return false;

	auto Doors = CurMap->GetDoors();
	for (auto iter = Doors.begin(); iter != Doors.end(); iter++) {
		auto _Door = iter->second;
		if (_Door->GetCollision() != _This)
			continue;
		if (_Door->GetState().GetCurStateName() == "Battle")
			continue;
		
		Index = iter->first;
	}

	if (Index == -1)
		return true;

	MapMoveThread = std::thread(&MapManager::MoveRoom, this, (Index + 1) * 2);
	MapMoveThread.detach();
	
	return true;
}

void MapManager::LoadMapFormData() {
	const char* items[] = { "Room", "Start", "Boss", "Shop", "Gold" };
	for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
		GameEngineDirectory Dir;
		Dir.MoveParentToExitsChildDirectory("GameEngineRoomData");
		Dir.Move("GameEngineRoomData");
		Dir.Move(items[i]);

		std::vector<GameEngineFile> MapDatas = Dir.GetAllFile();
		std::vector<std::string> JsonData;

		for (size_t j = 0; j < MapDatas.size(); j++) {
			//Json::Reader

			std::ifstream TempFile(MapDatas[j].GetFullPath());

			JsonData.push_back(MapDatas[j].GetFullPath());
		}
		MapFormData.insert(make_pair(i + 1, JsonData));
	}
}

void MapManager::RandomMapGenerate(GameEngineLevel* _Level) {
	GameEngineFontRenderer* Font = CreateComponent<GameEngineFontRenderer>();
	Font->SetRenderingOrder(10000);
	Font->SetColor({ 1.0f, 1.0f, 1.0f, 1.0 });
	Font->SetSize(20);
	Font->SetScreenPostion({ 0, 30 });
	Font->ChangeCamera(CAMERAORDER::UICAMERA);

	srand((unsigned)time(NULL));

	LoadMapFormData();

	//인접 칸이 이미 채워진 경우 포기합니다.
	//만약 인접 칸 자체에 둘 이상의 채워진 인접 칸이 있다면, 포기합니다.
	//이미 충분한 방을 생성하였다면 포기합니다.
	//50 % 확률로 포기합니다.
	//그 외 경우, 인접 칸에 방을 만들고, 해당 칸을 큐에 넣습니다.
	while (true) {
		RoomPlan = new int[Width * Height];
		for (int i = 0; i < Width * Height; i++) {
			RoomPlan[i] = 0;
		}
		EndRoom.clear();
		RoomQueue = queue<int>();
		RoomCount = 0;

		Visit(StartPos);
		while (RoomQueue.size() > 0) {
			int i = RoomQueue.front();
			RoomQueue.pop();
			int x = i % Width;
			bool created = false;

			if (x > 1) created = (WORD)created | (WORD)Visit(i - 1);
			if (x < 9) created = (WORD)created | (WORD)Visit(i + 1);
			if (i > 20) created = (WORD)created | (WORD)Visit(i - Width);
			if (i < 70) created = (WORD)created | (WORD)Visit(i + Width);
			if (!created) {
				EndRoom.push_back(i);
			}
		}

		if (RoomCount < MinRoomCount) {
			continue;
		}

		if (EndRoom.size() < SpecialRoomCount) {
			continue;
		}

		for (int i = 0; i < SpecialRoomCount; i++) {
			RoomPlan[EndRoom.back()] = (int)GameRoomType::Boss + i;
			EndRoom.pop_back();
		}

		RoomPlan[StartPos] = (int)GameRoomType::Start;
		string temp = "";
		for (int i = 0; i < Height; i++) {
			for (int j = 0; j < Width; j++) {
				if (RoomPlan[i * Width + j] == (int)GameRoomType::Start)
					temp += "S";
				else
					temp += to_string(RoomPlan[i * Width + j]);
			}
			temp += '\n';
		}
		//Font->SetText(temp);

		for (int i = 0; i < Height * Width; i++) {
			int RoomType = RoomPlan[i];
			if (RoomType == (int)GameRoomType::Wall) {
				continue;
			}

			if (MapFormData[RoomType].empty()) {
				MsgBoxAssert("방 샘플이 존재하지 않습니다.");
				return;
			}

			int RoomPlanCount = MapFormData.at(RoomType).size();
			int RoomIndex = rand() % RoomPlanCount;

			// 만들어져 있는 방의 구조 중 하나를 가져온다
			std::ifstream TempFile(MapFormData.at(RoomType)[RoomIndex]);
			Json::Value Root;

			Json::Reader Reader;
			Reader.parse(TempFile, Root);

			TempFile.close();

			// 맵 오브젝트를 만들어 그 방의 구조에 따라 오브젝트를 넣는다
			PlayMap* TempMap = _Level->CreateActor<PlayMap>(ROOM(i));
			TempMap->RoomType = RoomType;

			float4 MapSize = { GameEngineWindow::GetScale().x, GameEngineWindow::GetScale().y };
			float4 FixedPosition = { MapSize.x * (i % Width), -MapSize.y * (i / Height) };
			TempMap->GetTransform().SetWorldPosition(FixedPosition);
			TempMap->Initialize();

			if (RoomType == (int)GameRoomType::Start) {
				TempMap->GetRenderer()->SetTexture("start_000.png");
				Player::GetMainPlayer()->GetTransform().SetWorldPosition(FixedPosition);
			}
			TempMap->Off();

			PlayMaps.insert(make_pair(i, TempMap));

			for (int j = 0; j < 7 * 13; j++) {
				if (Root[to_string(j)].empty())
					continue;

				Json::Value Info = Root[to_string(j)];
				int index = Info["Index"].asInt();

				GameEngineActor* NewActor = GameEngineInstanceFactory::CreateActor(_Level, Info["Type"].asInt(), i);
				float x, y;
				x = (int)index % 13;
				y = (int)(index / 13);
				float4 Start = { -338.f, 165.f };
				float4 Pos = { 52 * x + 52.f / 2.f, -(52 * y + 52.f / 2.f) };
				//NewActor->SetParent(TempMap);
				NewActor->GetTransform().SetWorldPosition(FixedPosition + Start + Pos);
				NewActor->Off();

				TempMap->GetIncludeActors().push_back(NewActor);
			}
		}

		delete RoomPlan;

		break;
	}

	MoveRoom();
}

void MapManager::MoveRoom(int _Direction) {
	int NextRoomIndex = CurRoomIndex;
	switch (_Direction) {
	case Direction::Down:
		NextRoomIndex += 10;
		break;
	case Direction::Left:
		NextRoomIndex--;
		break;
	case Direction::Right:
		NextRoomIndex++;
		break;
	case Direction::Up:
		NextRoomIndex -= 10;
		break;
	}

	if (PlayMaps.find(NextRoomIndex) == PlayMaps.end())
		return;

	int OpenCheck[4] = { 0, };
	for (int i = 0; i < _countof(Move); i++) {
		if (PlayMaps.find(NextRoomIndex + Move[i]) == PlayMaps.end()) {
			continue;
		}
		PlayMap* NextRoom = PlayMaps[NextRoomIndex];

		OpenCheck[i] = NextRoom->RoomType;
	}

	PlayMap* NewMap = PlayMaps.at(NextRoomIndex);
	NewMap->MapOn(OpenCheck);

	float4 DestPosition = NewMap->GetTransform().GetWorldPosition() + float4::BACK * 1000.f;
	if (_Direction == 0) {
		GetLevel()->GetMainCameraActorTransform().SetWorldPosition(DestPosition);
	} else {
		//Player::GetMainPlayer()->GetCollision()->Off();
		GetLevel()->GetMainCameraActor()->MapChange(DestPosition);
	}

	CurRoomIndex = NextRoomIndex;

	// 맵이 바뀌었으니 새 맵을 가져온다.
	auto& Doors = GetCurrentMap()->GetDoors();

	int FixDir[4] = {
		3, 2, 1, 0
	};

	int Index = _Direction / 2 - 1;

	if (Index == -1)
		return;

	auto NewDoor = Doors[FixDir[Index]];
	if (NewDoor == nullptr)
		return;

	auto NewPos = NewDoor->GetSpawnTransform()->GetWorldPosition();
	NewPos.z = 0.f;
	Player::GetMainPlayer()->GetTransform().SetWorldPosition(NewPos);
}

void MapManager::MoveRoomPos(float4 _DestPos, int _Direction) {
	MoveRoom(_Direction);
}

void MapManager::TurnOffLastRoom() {
	if (LastRoomIndex == -1)
		return;

	auto _LastRoom = GetMap(LastRoomIndex);
	if (_LastRoom == nullptr) {
		return;
	}

	_LastRoom->MapOff();
}

bool MapManager::Visit(int i) {
	if (RoomPlan[i])
		return false;

	int neighbors = NCount(i);
	if (neighbors > 1)
		return false;

	if (RoomCount >= MaxRoomCount)
		return false;

	if (rand() % 2 && i != StartPos)
		return false;

	RoomQueue.push(i);
	RoomPlan[i] = 1;
	RoomCount++;

	return true;
}

int MapManager::NCount(int i) {
	return RoomPlan[i - Width] + RoomPlan[i + Width] + RoomPlan[i - 1] + RoomPlan[i + 1];
}