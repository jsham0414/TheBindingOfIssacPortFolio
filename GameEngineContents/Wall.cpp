#include "PreCompile.h"
#include "Wall.h"

Wall::Wall() {
	SetName("Wall");
}

Wall::~Wall() {
}

void Wall::Start() {
	//Renderer = CreateComponent<GameEngineTextureRenderer>();
	Collision = CreateComponent<GameEngineCollision>();
	float4 Color = float4::RED;
	Color.a = 0.2f;
	Collision->SetDebugSetting(CollisionType::CT_AABB2D, Color);

	Collision->GetTransform().SetStatic(true);
}

bool Wall::PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
	return true;
}

void Wall::CreateFrameAnimation() {
}

void Wall::Update(float _DeltaTime) {
}
