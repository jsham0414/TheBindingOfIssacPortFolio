#pragma once
#include "GameEngineFont.h"
#include "GameEngineDefaultRenderer.h"
#include "GameEngineRenderTarget.h"

enum class LeftAndRightSort
{
	LEFT = 0x0,
	CENTER = 0x1,
	RIGHT = 0x2,
};

enum class TopAndBotSort
{
	TOP = 0x0,
	VCENTER = 0x4,
	BOTTOM = 0x8,
};

// ���� :
class GameEngineDevice;
class GameEngineFontRenderer : public GameEngineDefaultRenderer
{
	friend GameEngineDevice;

public:
	// constrcuter destructer
	GameEngineFontRenderer();
	~GameEngineFontRenderer();

	// delete Functionq
	GameEngineFontRenderer(const GameEngineFontRenderer& _Other) = delete;
	GameEngineFontRenderer(GameEngineFontRenderer&& _Other) noexcept = delete;
	GameEngineFontRenderer& operator=(const GameEngineFontRenderer& _Other) = delete;
	GameEngineFontRenderer& operator=(GameEngineFontRenderer&& _Other) noexcept = delete;

	void SetLeftAndRightSort(LeftAndRightSort _Value)
	{
		LR = _Value;
	}

	void SetTopAndBotSort(TopAndBotSort _Value)
	{
		TB = _Value;
	}

	void SetText(const std::string& _Text, const std::string& _Font = "����");

	std::string GetText() 
	{
		return Text;
	}

	void SetScreenPostion(float4 _ScreenPostion)
	{
		ScreenPostion = _ScreenPostion;
	}

	float4 GetScreenPostion()
	{
		return ScreenPostion;
	}

	void SetColor(float4 _Color)
	{
		Color = _Color;
	}

	void SetSize(float _Size)
	{
		FontSize = _Size;
	}

protected:
	virtual void Start();

	virtual void Render(float _DeltaTime);

	static GameEngineRenderTarget* FontTarget;

	std::string Text;
	GameEngineFont* Font;
	float FontSize;
	float4 Color;
	float4 ScreenPostion;

	LeftAndRightSort LR;
	TopAndBotSort TB;
};

