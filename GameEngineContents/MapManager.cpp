#include "PreCompile.h"
#include "MapManager.h"
#include <GameEngineContents/Enums.h>
#include <fstream>
#include <GameEngineContents/Door.h>
#include <GameEngineContents/Wall.h>
#include "Minimap.h"
#include <GameEngineContents/DropItem.h>
#include <GameEngineContents/PopUpMessage.h>

//const int MapManager::Move[4] 

MapManager::MapManager() : RoomWidth(13), RoomHeight(7) {
	Width = 10;
	Height = 10;

	RoomCount = 0;
	MaxRoomCount = 15;
	MinRoomCount = 7;

	StartPos = 45;

	SpecialRoomCount = 3;
	LastRoomIndex = -1;

	StartOffset = { -338.f, 165.f };
	TileSize = 52;

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 13; j++) {
			ObjectSpawnType[i][j] = 0;
		}
	}
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
	
	if (PlayMaps.empty() || PlayMaps.find(_Index) == PlayMaps.end()) {
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

	Result = CurMap->GetMapEnemyCount();

	return Result;
}

// 방에 몬스터가 남아있는지 확인
void MapManager::CheckEnemies(bool _Forced) {
	PlayMap* CurMap = GetCurrentMap();
	if (CurMap == nullptr)
		return;

	std::string NextState;
	auto& CurrentState = CurMap->GetState();
	if (GetCurMapEnemyCount() <= 0) {
		NextState = "Idle";
	} else {
		NextState = "Battle";
	}

	if (NextState != CurrentState.GetCurStateName() || (_Forced)) {
		if (_Forced == false && NextState == "Idle" && GetCurrentMap()->RoomType == (int)GameRoomType::Room) {
			auto Item = GetLevel()->CreateActor<DropItem>();
			Item->SetType(rand() % (int)StuffType::End);
			float4 NewPos = GetCurrentMap()->GetTransformData().WorldPosition;
			NewPos.z = 0.f;
			Item->GetTransform().SetWorldPosition(NewPos);
		}

		CurrentState.ChangeState(NextState);
	}
}

void MapManager::ObjectDestory(GameEngineActor* _Actor) {
	PlayMap* CurMap = GetCurrentMap();
	if (CurMap == nullptr)
		return;

	auto& ActorList = CurMap->GetIncludeActors();

	auto FindResult = std::find(ActorList.begin(), ActorList.end(), _Actor);
	if (FindResult == ActorList.end())
		return;

	ActorList.erase(FindResult);

	_Actor->Death();

	CheckEnemies();
}

int MapManager::GetRoomType(int _Index) {
	auto Map = GetMap(_Index);
	if (Map == nullptr)
		return -1;

	return Map->RoomType;
}

void MapManager::Update(float _DeltaTime) {
	PlayMap* CurMap = GetCurrentMap();
	if (CurMap == nullptr)
		return;

	auto Func = std::bind(&MapManager::PlayerCollision, this, std::placeholders::_1, std::placeholders::_2);
	CurMap->UpdateCollision(Func);
}

bool MapManager::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	if (GetLevel()->GetMainCameraActor()->IsMoving())
		return false;

	LastRoomIndex = CurRoomIndex;
	PlayMap* CurMap = GetCurrentMap();
	if (CurMap == nullptr)
		return false;

	int Index = -1;
	auto& Doors = CurMap->GetDoors();
	for (int i = 0; i < Doors.size(); i++) {
		auto _Door = Doors[i];
		if (_Door->GetCollision() != _This)
			continue;
		if (_Door->GetState().GetCurStateName() == "Battle")
			continue;

		Index = i;
	}

	if (Index == -1)
		return true;

	MoveRoom((Index + 1) * 2);
	
	return true;
}

void MapManager::LoadMapFormData() {
	const char* items[] = { "Room", "Start", "Boss", "Shop", "Gold" };
	for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
		GameEngineDirectory Dir;
		Dir.MoveParentToExitsChildDirectory("GameEngineData");
		Dir.Move("GameEngineData");
		Dir.Move("MapForm");
		Dir.Move(items[i]);

		std::vector<GameEngineFile> MapDatas = Dir.GetAllFile();
		std::vector<std::string> JsonData;

		for (size_t j = 0; j < MapDatas.size(); j++) {
			std::string FormFilePath = MapDatas[j].GetFullPath();
			JsonData.push_back(FormFilePath);
		}
		MapFormData.insert(make_pair(i + 1, JsonData));
	}
}

int MapManager::GetObjectSpawnType(int _X, int _Y) {
	return ObjectSpawnType[_Y][_X];
}

void MapManager::RandomMapGenerate(GameEngineLevel* _Level) {
	GameEngineFontRenderer* Font = CreateComponent<GameEngineFontRenderer>();
	Font->SetRenderingOrder(10000);
	Font->SetColor({ 1.0f, 1.0f, 1.0f, 1.0 });
	Font->SetSize(20);
	Font->SetScreenPostion({ 100, 100 });
	Font->ChangeCamera(CAMERAORDER::UICAMERA);

	LoadMapFormData();

	//인접 칸이 이미 채워진 경우 포기합니다.
	//만약 인접 칸 자체에 둘 이상의 채워진 인접 칸이 있다면, 포기합니다.
	//이미 충분한 방을 생성하였다면 포기합니다.
	//50% 확률로 포기합니다.
	//그 외 경우, 인접 칸에 방을 만들고, 해당 칸을 큐에 넣습니다.
	while (true) {
		std::unordered_map<int, int> RoomPlan;
		for (int i = 0; i < Width * Height; i++) {
			RoomPlan[i] = 0;
		}
		EndRoom.clear();
		RoomQueue = queue<int>();
		RoomCount = 0;

		Visit(RoomPlan, StartPos);
		while (RoomQueue.size() > 0) {
			int i = RoomQueue.front();
			RoomQueue.pop();
			int x = i % Width;
			bool created = false;

			if (x > 1) created = (WORD)created | (WORD)Visit(RoomPlan, i - 1);
			if (x < 9) created = (WORD)created | (WORD)Visit(RoomPlan, i + 1);
			if (i > 20) created = (WORD)created | (WORD)Visit(RoomPlan, i - Width);
			if (i < 70) created = (WORD)created | (WORD)Visit(RoomPlan, i + Width);
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
		Font->SetText(temp);

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

			GameRoomType AdjacenceRoom[4];
			for (int l = 0; l < 4; l++) {
				AdjacenceRoom[l] = static_cast<GameRoomType>(RoomPlan[i + Move[l]]);
			}

			TempMap->SetType(static_cast<GameRoomType>(RoomPlan[i]));
			TempMap->Initialize(AdjacenceRoom);

			if (RoomType == (int)GameRoomType::Start) {
				Player::GetMainPlayer()->GetTransform().SetWorldPosition(FixedPosition);
			}
			TempMap->Off();

			PlayMaps.insert(make_pair(i, TempMap));

			for (int j = 0; j < RoomWidth * RoomHeight; j++) {
				if (Root[to_string(j)].empty())
					continue;
				
				Json::Value Info = Root[to_string(j)];
				int index = j;

				auto NewActor = GameEngineInstanceFactory::CreateActor(_Level, Info["Type"].asInt(), i);

				int x, y;
				x = (int)index % RoomWidth;
				y = (int)(index / RoomWidth);

				if (NewActor->GetCollision() != nullptr)
					ObjectSpawnType[y][x] = NewActor->GetCollision()->GetOrder();

				float4 Pos = { TileSize * x + TileSize / 2.f, -(TileSize * y + TileSize / 2.f), 10.f };
				
				NewActor->GetTransform().SetWorldPosition(FixedPosition + StartOffset + Pos);
				NewActor->Off();

				TempMap->GetIncludeActors().push_back(NewActor);
			}
		}

		break;
	}

	MoveRoom();
	MinimapUI = GetLevel()->CreateActor<Minimap>();

	PopUpMessage* StageMessage = GetLevel()->CreateActor<PopUpMessage>();
	StageMessage->SetTitle("Stage 1");
	StageMessage->Play();
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

	// 다음 맵의 어느 문이 열려있어야 하는지 구한다.
	int OpenCheck[4] = { 0, };
	for (int i = 0; i < _countof(Move); i++) {
		if (PlayMaps.find(NextRoomIndex + Move[i]) == PlayMaps.end()) {
			continue;
		}
		PlayMap* NextRoom = PlayMaps[NextRoomIndex];

		OpenCheck[i] = NextRoom->RoomType;
	}

	PlayMap* NextMap = PlayMaps.at(NextRoomIndex);
	NextMap->MapOn(OpenCheck);


	CurRoomIndex = NextRoomIndex;

	// 맵이 바뀌었으니 새 맵을 가져온다.
	auto& Doors = GetCurrentMap()->GetDoors();

	int FixDir[4] = {
		3, 2, 1, 0
	};

	float4 DestPosition = NextMap->GetTransform().GetWorldPosition();
	if (_Direction == 0) { // 시작방에서 시작할 때
		CheckEnemies();
		GetLevel()->GetMainCameraActorTransform().SetWorldPosition(DestPosition);
	} else {
		MinimapUI->MoveMap(CurRoomIndex);

		int Index = _Direction / 2 - 1;
		auto NewDoor = Doors[FixDir[Index]]; // 왼쪽 문으로 들어갔으면 다음 방 오른쪽 문에서 나올 수 있게 한다.
		if (NewDoor == nullptr)
			return;

		// 문마다 달려있는 워프 Transform이다.
		auto NewPos = NewDoor->GetSpawnTransform()->GetWorldPosition();
		
		// 겉으로는 열려있었지만 벽이다.
		GetCurrentMap()->SetDoorState("FakeOpen");

		NewPos.z = 0.f;
		Player::GetMainPlayer()->GetTransform().SetWorldPosition(NewPos);

		GetLevel()->GetMainCameraActor()->CameraMove(DestPosition, 0.5f); // 벡터와 도착하는데 걸릴 시간
	}
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

bool MapManager::Visit(std::unordered_map<int, int>& _RoomPlan, int i) {
	if (_RoomPlan[i])
		return false;

	int neighbors = NCount(_RoomPlan, i);
	if (neighbors > 1)
		return false;

	if (RoomCount >= MaxRoomCount)
		return false;

	if (rand() % 2 && i != StartPos)
		return false;

	RoomQueue.push(i);
	_RoomPlan[i] = 1;
	RoomCount++;

	return true;
}

int MapManager::NCount(std::unordered_map<int, int>& _RoomPlan, int i) {
	return _RoomPlan[i - Width] + _RoomPlan[i + Width] + _RoomPlan[i - 1] + _RoomPlan[i + 1];
}