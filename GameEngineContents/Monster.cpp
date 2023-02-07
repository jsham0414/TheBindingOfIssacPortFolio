#include "PreCompile.h"
#include "Monster.h"
#include <GameEngineCore/GameEngineDefaultRenderer.h>

Monster::Monster()
{
}

Monster::~Monster()
{
}

void Monster::DamageAnimation() {
	float4 _OriginalColor = float4::WHITE;
	float4& _MulColor = Renderer->GetPixelData().MulColor;

	Mutex.lock();
	_MulColor = float4::RED;
	Mutex.unlock();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	//for (int i = 0; i < 2; i++) {
	//	Mutex.lock();
	//	_MulColor = float4::WHITE;
	//	_MulColor.a = 0.1f;
	//	Mutex.unlock();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(30));

	//	Mutex.lock();
	//	_MulColor = float4::YELLOW;
	//	Mutex.unlock();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(30));

	//	Mutex.lock();
	//	_MulColor = float4::RED;
	//	Mutex.unlock();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	//}

	Mutex.lock();
	_MulColor = _OriginalColor;
	Mutex.unlock();
	//Renderer->ShaderResources.AllResourcesReset();
}
