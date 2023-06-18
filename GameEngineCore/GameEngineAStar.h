#pragma once
#include "GameEngineTransformComponent.h"
#include <queue>
#include <stack>

class GameEngineAStarNode {
public:
	int F, G, X, Y;
	GameEngineAStarNode(int _F, int _G, int _X, int _Y) {
		this->F = _F;
		this->G = _G;
		this->X = _X;
		this->Y = _Y;
	}
	GameEngineAStarNode(const GameEngineAStarNode& Node) {
		this->F = Node.F;
		this->G = Node.G;
		this->X = Node.X;
		this->Y = Node.Y;
	}
	GameEngineAStarNode(GameEngineAStarNode&& _Other) {
		this->F = _Other.F;
		this->G = _Other.G;
		this->X = _Other.X;
		this->Y = _Other.Y;
	}
	GameEngineAStarNode& operator=(GameEngineAStarNode&& _Other) {
		this->F = _Other.F;
		this->G = _Other.G;
		this->X = _Other.X;
		this->Y = _Other.Y;
	}
};

class GameEngineAStar : public GameEngineTransformComponent {
public:
	// constrcuter destructer
	GameEngineAStar();
	~GameEngineAStar();

	// delete Function
	GameEngineAStar(const GameEngineAStar& _Other) = delete;
	GameEngineAStar(GameEngineAStar&& _Other) noexcept = delete;
	GameEngineAStar& operator=(const GameEngineAStar& _Other) = delete;
	GameEngineAStar& operator=(GameEngineAStar&& _Other) noexcept = delete;

	void AStarStart(MapManager* _Manager, float4 _DestPosition);

	bool Over;

	std::stack<float4> Path;
private:
	float4 GetPlayerTilePosition(float4 _RoomPosition);

	float4 GetTargetTilePosition(float4 _RoomPosition, float4 _TargetPosition);

	float4 GetTilePosition(float4 _RoomPosition);

	float4 GetTargetTilePosition(float4 _TargetPosition);

	float4 GetTilePosition();

	float4 GetNextStep();
	int GetDistanceValue(int _X, int _Y);
};

