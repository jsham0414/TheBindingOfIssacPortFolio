#include "PreCompile.h"
#include "MapObjectBatcher.h"

MapObjectBatcher::MapObjectBatcher() {
	Width = 13;
	Height = 7;
}

MapObjectBatcher::~MapObjectBatcher() {

}

void MapObjectBatcher::Start() {
	StartPos = { -454.f + 142, 244.f - 88 };
	float4 newPos;
	TexScale = GameEngineTexture::Find("Frame_0.png")->GetScale();

	GameEngineTextureRenderer* Temp = CreateComponent<GameEngineTextureRenderer>();
	Temp->SetTexture("room_000.png");
	newPos = { 0, 0 };
	Temp->GetTransform().SetWorldPosition(newPos);
	Temp->GetTransform().SetWorldScale({ Temp->GetCurTexture()->GetScale().x, Temp->GetCurTexture()->GetScale().y });

	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			Temp = CreateComponent<GameEngineTextureRenderer>();
			Temp->SetTexture("Frame_0.png");
			newPos = { StartPos.x + j * TexScale.x, StartPos.y - i * TexScale.y };
			Temp->GetTransform().SetWorldPosition(newPos);
			Temp->GetTransform().SetWorldScale({ TexScale.x, TexScale.y });

			if (i == Height - 1) {
				Temp = CreateComponent<GameEngineTextureRenderer>();
				Temp->SetTexture("Frame_1.png");
				newPos = { StartPos.x + j * TexScale.x, StartPos.y - i * TexScale.y };
				Temp->GetTransform().SetWorldPosition(newPos);
				Temp->GetTransform().SetWorldScale({ TexScale.x, TexScale.y });
			}
			if (j == Width - 1) {
				Temp = CreateComponent<GameEngineTextureRenderer>();
				Temp->SetTexture("Frame_2.png");
				newPos = { StartPos.x + j * TexScale.x, StartPos.y - i * TexScale.y };
				Temp->GetTransform().SetWorldPosition(newPos);
				Temp->GetTransform().SetWorldScale({ TexScale.x, TexScale.y });
			}
		}
	}

}

void MapObjectBatcher::Update(float _DeltaTime) {

}

void MapObjectBatcher::BatchObject(int _Index, ObjectBatchInfo* _Info) {
	if (BatchData.contains(_Index))
		return;

	BatchDummy* Dummy = GetLevel()->CreateActor<BatchDummy>(OBJECTORDER::TileMapObject);
	float4 Pos = { StartPos.x + (_Index % Width) * TexScale.x, StartPos.y - (int)(_Index / Width) * TexScale.y };
	Dummy->GetTransform().SetWorldPosition(Pos);
	Dummy->SetName(_Info->Name);
	Dummy->GetRenderer()->SetTexture(_Info->Renderer->GetCurTexture()->GetNameConstPtr(), 0);
	Dummy->GetRenderer()->GetTransform().SetWorldScale(_Info->Renderer->GetTransform().GetWorldScale());
	Dummy->SetType(_Info->Index);
	BatchData[_Index] = Dummy;
}

void MapObjectBatcher::DeleteObject(int _Index) {
	if (!BatchData.contains(_Index))
		return;

	BatchData[_Index]->Death();
	BatchData.erase(_Index);
}
