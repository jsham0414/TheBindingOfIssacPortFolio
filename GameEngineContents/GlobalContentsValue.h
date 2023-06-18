#pragma once
#include "Player.h"
#include "Monster.h"

enum class OBJECTORDER {
	TileMap,
	Player,
	Monster,
	Projectile,
	TileMapObject,
	Manager,
	Wall,
	AirWall,
	Bomb,
	Static,
	
	UI = 1000,
};

//enum class COLLISIONORDER
//{
//	Player,
//	Monster,
//	UI = 1000,
//};


// Ό³Έν :
class GlobalContentsValue
{
public:
	class Enums 
	{
	};

	class Actors 
	{
	public:
		static Monster* TestMonster;

	private:
		Actors() {		}
		~Actors() {		}
	};

private:
	GlobalContentsValue()	{	}

	~GlobalContentsValue()	{	}
};

