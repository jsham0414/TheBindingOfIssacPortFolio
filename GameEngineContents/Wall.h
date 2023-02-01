#pragma once

class Wall : public GameEngineActor {
public:
	Wall();
	~Wall();

	// delete Function
	Wall(const Wall& _Other) = delete;
	Wall(Wall&& _Other) noexcept = delete;
	Wall& operator=(const Wall& _Other) = delete;
	Wall& operator=(Wall&& _Other) noexcept = delete;

protected:
	void Start() override;
	bool PlayerCollision(GameEngineCollision* _This, GameEngineCollision* _Other);
	void CreateFrameAnimation();
	void Update(float _DeltaTime);
	void End() {}
};

