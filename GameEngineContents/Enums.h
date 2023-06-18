#pragma once

enum Direction {
	Down = 2,
	Left = 4,
	Right = 6,
	Up = 8,
};

enum class GameObjectGroup {
	Player,
	Monster,
	UI,
};

enum class GameBloodType {
	TINY,
	SMALL,
	MEDIUM,
	LARGE,
	HUGE,
};

enum class GameRoomType {
	Wall,
	Room,
	Start,
	Boss,
	Shop,
	Gold,
	End,
};

enum class StuffType {
	Gold,
	Bomb,
	Key,
	Heart,
	End,
};
