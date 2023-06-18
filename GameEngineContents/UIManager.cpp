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

	int PlayerMaxHp = Player::GetMainPlayer()->GetInfo().MaxHP;
	int PlayerHp = Player::GetMainPlayer()->GetInfo().HP;
	for (int i = 0; i < PlayerMaxHp / 2; i++) {
		HpInstance* _HpInst = GetLevel()->CreateActor<HpInstance>();
		_HpInst->GetRenderer()->SetScaleRatio(1.f);
		_HpInst->SetIndex(i);
		_HpInst->SetParent(this);
		HpList.AddNode(_HpInst);
	}

	SetHp(PlayerHp);

	float4 FixedPosition = { 57.f, -105.f };
	for (int i = 0; i < static_cast<int>(StuffType::Heart); i++) {
		auto Stuff = GetLevel()->CreateActor<StuffGuider>();
		Stuff->GetRenderer()->SetTexture("Stuffs.png", i);
		Stuff->GetRenderer()->SetScaleRatio(1.f);
		Stuff->GetRenderer()->ScaleToCutTexture(i);
		Stuff->GetTransform().SetLocalPosition({ FixedPosition.x, FixedPosition.y - (i * 25.f) });
		Stuff->ChangeValue(Player::GetMainPlayer()->GetStuff(i));
		Stuff->SetParent(this);

		Stuffs[i] = Stuff;
	}

	//BossUIInstance = GetLevel()->CreateActor<BossUI>();

	//BombRenderer->
}

void UIManager::SetHp(int _Hp) {
	int PlayerMaxHp = Player::GetMainPlayer()->GetInfo().MaxHP;
	int PlayerHp = _Hp;

	auto CurNode = HpList.GetHead();
	for (int i = 0; i < PlayerMaxHp / 2; i++) {
		if (CurNode == nullptr) {
			HpInstance* _HpInst = GetLevel()->CreateActor<HpInstance>();
			_HpInst->GetRenderer()->SetScaleRatio(1.f);
			_HpInst->SetIndex(i);
			_HpInst->SetParent(this);
			CurNode = HpList.AddNode(_HpInst);
		}
		CurNode = CurNode->NextNode;
	}

	Node<HpInstance*>* _HpInst = HpList.GetHead();
	for (int i = 0; i < PlayerMaxHp / 2; i++) {
		int _NewHp = PlayerHp - (i) * 2;

		_HpInst->Data->SetHp(_NewHp);

		_HpInst = _HpInst->NextNode;
	}
}

void UIManager::SetStuff(int _Type, int _Value) {
	Stuffs[_Type]->ChangeValue(_Value);
}

void UIManager::Update(float _DeltaTime) {

}
