#include "PreCompile.h"
#include "UIManager.h"

UIManager::UIManager() {
}

UIManager::~UIManager() {
}

void UIManager::Start() {
	int PlayerMaxHp = Player::GetMainPlayer()->GetMaxHp();
	int PlayerHp = Player::GetMainPlayer()->GetHp();
	for (int i = 0; i < PlayerMaxHp / 2; i++) {
		HpInstance* _HpInst = GetLevel()->CreateActor<HpInstance>();
		_HpInst->SetIndex(i);
		HpList.AddNode(_HpInst);
	}

	SetHp(PlayerHp);

	float4 FixedPosition = { 0, 0 };
	for (int i = 0; i < static_cast<int>(StuffType::End); i++) {
		auto Stuff = GetLevel()->CreateActor<StuffGuider>();
		Stuff->GetRenderer()->SetTexture("Stuffs.png", i);
		Stuff->GetRenderer()->ScaleToCutTexture(i);
		Stuff->GetTransform().SetWorldPosition({ FixedPosition.x, FixedPosition.y - (i * 25.f) });

		Stuffs[i] = Stuff;
	}
	//BombRenderer->
}

void UIManager::SetHp(int _Hp) {
	int PlayerMaxHp = Player::GetMainPlayer()->GetMaxHp();
	int PlayerHp = _Hp;

	Node<HpInstance*>* _HpInst = HpList.GetHead();
	for (int i = 0; i < PlayerMaxHp / 2; i++) {
		int _Hp = PlayerHp - (i) * 2;

		_HpInst->Data->SetHp(_Hp);

		_HpInst = _HpInst->NextNode;
	}
}

void UIManager::Update(float _DeltaTime) {

}
