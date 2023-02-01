#pragma once

class BatchDummy : public GameEngineActor {
public:
	BatchDummy();
	~BatchDummy();

	int GetType() { return Type; }
	void SetType(int _Type) { Type = _Type; }

protected:
	void Start() override;
	void Update(float _DeltaTime) {};
	void End() {}

	//GameEngineTextureRenderer* Renderer;
private:
	int Type;
};

