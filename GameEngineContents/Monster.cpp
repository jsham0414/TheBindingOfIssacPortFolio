#include "PreCompile.h"
#include "Monster.h"
#include "MapManager.h"
#include <GameEngineCore/GameEngineDefaultRenderer.h>
#include <GameEngineContents/DyingBlood.h>

Monster::Monster()
{
	BloodType = GameBloodType::MEDIUM;
	Dead = false;
}

Monster::~Monster()
{
}

void Monster::Damage(float _Value) {
	Hp -= _Value;

	if (Hp <= 0 && !IsDeath()) {
		auto Blood = GetLevel()->CreateActor<DyingBlood>();
		Blood->GetTransform().SetWorldPosition(GetTransformData().WorldPosition);
		Blood->SetType(BloodType);
		GetLevel()->GetMapManager()->ObjectDestory(this);
		return;
	}

	auto ColorChange = thread(&Monster::DamageAnimation, this);
	ColorChange.detach();
}

void Monster::DamageAnimation() {
	float4 _OriginalColor = float4::WHITE;
	auto DeathPtr = GetDeathPtr();

	Mutex.lock();
	if (*DeathPtr || Renderer == nullptr) {
		Mutex.unlock();
		return;
	}

	for (auto& _Renderer : GetConvertChilds<GameEngineTextureRenderer>()) {
		if (_Renderer == nullptr || _Renderer->GetNameConstRef() == "Shadow")
			continue;

		_Renderer->GetPixelData().MulColor = float4::RED;
	}
	Mutex.unlock();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	Mutex.lock();
	if (*DeathPtr || Renderer == nullptr) {
		Mutex.unlock();
		return;
	}

	for (auto& _Renderer : GetConvertChilds<GameEngineTextureRenderer>()) {
		if (_Renderer == nullptr || _Renderer->GetNameConstRef() == "Shadow") {
			continue;
		}

		_Renderer->GetPixelData().MulColor = _OriginalColor;
	}
	Mutex.unlock();
}
