#include "PreCompile.h"
#include "DamageBlood.h"

DamageBlood::DamageBlood() {
    AccumulateTime = 0.f;
    DeathTime = 4.f;
    DebugOff();
}

DamageBlood::~DamageBlood() {
}

void DamageBlood::Initialize() {
    Death(DeathTime);
    AccumulateTime = 0.f;
}

void DamageBlood::Start() {
    if (Renderer == nullptr) {
        Renderer = CreateComponent<GameEngineTextureRenderer>();
        Renderer->SetTexture("effect_016_bloodpool1.png", rand() % 12);
        Renderer->ScaleToCutTexture();

        Collision = CreateComponent<GameEngineCollision>();
        Collision->GetTransform().SetLocalScale({ 40.f, 40.f });
    }

    Initialize();
}

void DamageBlood::Update(float _DeltaTime) {
    float Alpha = 1.f - fmin(DeathTime, AccumulateTime) / DeathTime;
    Renderer->GetPixelData().MulColor.a = Alpha;

    AccumulateTime += _DeltaTime;

    Player* _Player = Player::GetMainPlayer();
    if (_Player->GetInvincible())
        return;

    Collision->IsCollision(CollisionType::CT_AABB2D, (int)OBJECTORDER::Player, CollisionType::CT_AABB2D,
        std::bind(&DamageBlood::MonsterCollision, this, std::placeholders::_1, std::placeholders::_2));
}

bool DamageBlood::MonsterCollision(GameEngineCollision* _This, GameEngineCollision* _Other) {
    if (_This->GetOrder() == _Other->GetOrder()) {
        return false;
    }
    
    if (_Other->GetOrder() == (int)OBJECTORDER::Player) {
        Player* _Player = static_cast<Player*>(_Other->GetActor());
        _Player->Assault();
    }
    return true;
}
