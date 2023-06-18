#pragma once
#include "../GameEngineContents/GlobalContentsValue.h"
#include "GameEngineLevel.h"
#include "GameEngineInstanceFactory.h"
#include "GameEngineUpdateObject.h"
#include "../GameEngineContents/Horf.h"
#include "../GameEngineContents/ItemHolder.h"
#include "../GameEngineContents/Monstro.h"
#include <GameEngineContents/Gaper.h>
#include <GameEngineContents/SlotMachine.h>

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
		switch (_Type) {
		case 0:
			Result = (GameEngineActor*)_Level->CreateActor<Horf>(ROOM(_RoomIndex));
			break;
		case 1:
			Result = (GameEngineActor*)_Level->CreateActor<ItemHolder>(ROOM(_RoomIndex));
			break;
		case 2:
			Result = (GameEngineActor*)_Level->CreateActor<Monstro>(ROOM(_RoomIndex));
			break;
		case 3:
			Result = (GameEngineActor*)_Level->CreateActor<Gaper>(ROOM(_RoomIndex));
			break;
		case 4:
			Result = (GameEngineActor*)_Level->CreateActor<SlotMachine>(ROOM(_RoomIndex));
			break;
		}

		return Result;
	}
};

