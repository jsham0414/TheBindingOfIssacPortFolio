#pragma once
#include "../GameEngineContents/GlobalContentsValue.h"
#include "GameEngineLevel.h"
#include "GameEngineInstanceFactory.h"
#include "GameEngineUpdateObject.h"
#include "../GameEngineContents/Horf.h"

#define ROOM(_Idx) 10000 + _Idx

class GameEngineLevel;
class GameEngineInstanceFactory final {
public:
	// constrcuter destructer
	GameEngineInstanceFactory();
	~GameEngineInstanceFactory();

	// delete Function
	GameEngineInstanceFactory(const GameEngineInstanceFactory& _Other) = delete;
	GameEngineInstanceFactory(GameEngineInstanceFactory&& _Other) noexcept = delete;
	GameEngineInstanceFactory& operator=(const GameEngineInstanceFactory& _Other) = delete;
	GameEngineInstanceFactory& operator=(GameEngineInstanceFactory&& _Other) noexcept = delete;

	//template <class T>
	static GameEngineActor* CreateActor(GameEngineLevel* _Level, int _Type, int _RoomIndex) {
		GameEngineActor* Result = nullptr;
		if (_Type == 0)
			Result = (GameEngineActor*)_Level->CreateActor<Horf>(ROOM(_RoomIndex));

		return Result;
	}
};

