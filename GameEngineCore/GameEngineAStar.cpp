#include "PreCompile.h"
#include "GameEngineAStar.h"
#include <GameEngineContents/MapManager.h>

GameEngineAStar::GameEngineAStar() {
	Over = false;
}

GameEngineAStar::~GameEngineAStar() {
}

float4 GameEngineAStar::GetPlayerTilePosition(float4 _RoomPosition) {
	float4 PlayerPosition = Player::GetMainPlayer()->GetTransformData().WorldPosition;
	float4 Result = _RoomPosition - PlayerPosition;
	// 138 83
	Result += { -286, 165.f };
	Result /= 52.f;

	int _X = static_cast<int>(floorf(Result.x));
	int _Y = static_cast<int>(floorf(Result.y));

	Result = { (float)_X, (float)_Y };
	return Result;
}

float4 GameEngineAStar::GetTargetTilePosition(float4 _RoomPosition, float4 _TargetPosition) {
	_RoomPosition -= {52.f * 6.f, 52.f * 3.f};
	float4 Result = _TargetPosition - _RoomPosition;
	// 138 83
	Result /= 52.f;

	int _X = static_cast<int>(floorf(Result.x));
	int _Y = static_cast<int>(floorf(Result.y));

	Result = { (float)_X, (float)_Y };
	return Result;
}

float4 GameEngineAStar::GetTilePosition(float4 _RoomPosition) {
	_RoomPosition -= {52.f * 6.f, 52.f * 3.f};
	float4 Position = GetTransformData().WorldPosition;
	float4 Result = Position - _RoomPosition;
	// 138 83
	Result += {0.f, -295.f};
	Result /= 52.f;

	int _X = static_cast<int>(floorf(Result.x));
	int _Y = static_cast<int>(floorf(Result.y));

	Result = { (float)_X, (float)_Y };
	return Result;
}

float4 GameEngineAStar::GetTargetTilePosition(float4 _TargetPosition) {
	float4 Result = _TargetPosition;
	// 138 83
	Result /= 52.f;

	int _X = static_cast<int>(floorf(Result.x));
	int _Y = static_cast<int>(floorf(Result.y));

	Result = { (float)_X, (float)_Y };
	return Result;
}

float4 GameEngineAStar::GetTilePosition() {
	float4 Position = GetTransformData().WorldPosition;
	float4 Result = Position;
	// 138 83
	Result /= 52.f;

	int _X = static_cast<int>(floorf(Result.x));
	int _Y = static_cast<int>(floorf(Result.y));

	Result = { (float)_X, (float)_Y };
	return Result;
}

float4 GameEngineAStar::GetNextStep() {
	return float4();
}

int GameEngineAStar::GetDistanceValue(int _X, int _Y) {
	//Open[std::make_pair(NextX, NextY)]
	return 0;
}

void GameEngineAStar::AStarStart(MapManager* _Manager, float4 _DestPosition) {
	//int DirX[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	//int DirY[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	//int Cost[] = { 14, 10, 14, 10, 10, 14, 10, 14 };
	int DirX[] = { 0, -1, 0, 1, -1, -1, 1, -1 };
	int DirY[] = { -1, 0, 1, 0, -1, 1, 1, -1 };
	int Cost[] = { 10, 10, 10, 10, 14, 14, 14, 14 };

	const int MaxWidth = 7;
	const int MaxHeight = 13;

	//float4 MapPosition = _Manager->GetCurrentMap()->GetTransformData().WorldPosition;
	//POINT Pos(GetTilePosition(MapPosition).x, GetTilePosition(MapPosition).y);
	//float4 TargetPosition = { 53.f, 53.f };
	//POINT DestPos(GetTargetTilePosition(MapPosition, MapPosition + TargetPosition + asdd).x, GetTargetTilePosition(MapPosition, MapPosition + TargetPosition + asdd).y);

	POINT Pos(GetTilePosition().x, GetTilePosition().y);
	POINT DestPos(GetTargetTilePosition(_DestPosition).x, GetTargetTilePosition(_DestPosition).y);

	std::map<std::pair<int, int>, int> Open;
	std::map<std::pair<int, int>, int> Closed;
	std::queue<GameEngineAStarNode> PQueue;
	std::map<std::pair<int, int>, POINT> Parent;

	Open[std::make_pair(DestPos.x, DestPos.y)] = 10 * (abs(DestPos.x - Pos.x) + abs(DestPos.y - Pos.y));
	GameEngineAStarNode asd(10 * (abs(DestPos.x - Pos.x) + abs(DestPos.y - Pos.y)), 0, Pos.x, Pos.y);
	PQueue.push(asd);
	Parent[std::make_pair(Pos.x, Pos.y)] = POINT(Pos.x, Pos.y);

	while (PQueue.size() > 0) {
		GameEngineAStarNode Node = PQueue.front();
		PQueue.pop();

		if (Closed[std::make_pair(Node.X, Node.Y)])
			continue;

		Closed[std::make_pair(Node.X, Node.Y)] = true;

		if (Node.Y == DestPos.y && Node.X == DestPos.x) {
			while (!Path.empty()) Path.pop();
			Over = true;
			POINT NextPos(DestPos.x, DestPos.y);
			while (true) {
				Path.push({ NextPos.x * 52.f, NextPos.y * 52.f });
				NextPos = Parent[std::make_pair(NextPos.x, NextPos.y)];

				if (Pos.x == NextPos.x && Pos.y == NextPos.y)
					break;
			}
			break;
		}

		for (int i = 0; i < _countof(DirX); i++) {
			int NextY = Node.Y + DirY[i];
			int NextX = Node.X + DirX[i];

			if (_Manager->GetObjectSpawnType(NextX, NextY) == (int)OBJECTORDER::Static)
				continue;
			if (Closed[std::make_pair(NextX, NextY)])
				continue;

			int G = Node.G + Cost[i];
			int H = 10 * (abs(DestPos.x - NextX) + abs(DestPos.y - NextY));

			if (Open.find(std::make_pair(NextX, NextY)) == Open.end())
				Open[std::make_pair(NextX, NextY)] = INT_MAX;

			if (Open[std::make_pair(NextX, NextY)] < G + H)
				continue;

			Open[std::make_pair(NextX, NextY)] = G + H;
			PQueue.push(GameEngineAStarNode(G + H, G, NextX, NextY));
			Parent[std::make_pair(NextX, NextY)] = POINT(Node.X, Node.Y);
		}
	}
}


