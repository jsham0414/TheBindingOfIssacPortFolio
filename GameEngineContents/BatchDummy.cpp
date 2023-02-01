#include "PreCompile.h"
#include "BatchDummy.h"

BatchDummy::BatchDummy() {
}

BatchDummy::~BatchDummy() {
}

void BatchDummy::Start() {
	Renderer = CreateComponent<GameEngineTextureRenderer>();
}
