#include "PreCompile.h"
#include "UIManager.h"

UIManager::UIManager() {
}

UIManager::~UIManager() {
}

void UIManager::Start() {
	GetTransform().SetWorldPosition({ 
		-GameEngineWindow::GetScale().x / 2.f, 
		GameEngineWindow::GetScale().y / 2.f 
		});

	int PlayerMaxHp = Player::GetMainPlayer()->GetMaxHp();
	int PlayerHp = Player::GetMainPlayer()->GetHp();
	for (int i = 0; i < PlayerMaxHp / 2; i++) {
		HpInstance* _HpInst = GetLevel()->CreateActor<HpInstance>();
		_HpInst->SetIndex(i);
		_HpInst->SetParent(this);
		HpList.AddNode(_HpInst);
	}

	SetHp(PlayerHp);

	float4 FixedPosition = { 57.f, -105.f };
	for (int i = 0; i < static_cast<int>(StuffType::End); i++) {
		auto Stuff = GetLevel()->CreateActor<StuffGuider>();
		Stuff->GetRenderer()->SetTexture("Stuffs.png", i);
		Stuff->GetRenderer()->ScaleToCutTexture(i);
		Stuff->GetTransform().SetLocalPosition({ FixedPosition.x, FixedPosition.y - (i * 25.f) });
		Stuff->ChangeValue(Player::GetMainPlayer()->GetStuff(i));
		Stuff->SetParent(this);

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

void UIManager::SetStuff(int _Type, int _Value) {
	Stuffs[_Type]->ChangeValue(_Value);
}

void UIManager::Update(float _DeltaTime) {

}
