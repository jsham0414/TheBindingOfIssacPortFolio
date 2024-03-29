#pragma once
#include "GameEngineDefaultRenderer.h"

enum class PIVOTMODE
{
	CENTER,
	BOT,
	TOP,
	LEFT,
	RIGHT,
	LEFTTOP,
	RIGHTTOP,
	LEFTBOT,
	RIGHTBOT,
	CUSTOM,
};

enum class SCALEMODE
{
	IMAGE,
	CUSTOM,
};

struct PixelData
{
	float4 MulColor;
	float4 PlusColor;
	float4 Slice;
	int FillType;

	PixelData()
		: MulColor(float4::WHITE)
		, PlusColor(float4::ZERO)
		, Slice(float4::ZERO)
		, FillType(0)
	{

	}
};

struct AtlasData 
{
public:
	float4 FrameData;
	float4 PivotPos;
};


class FrameAnimation_DESC 
{
public:
	std::string TextureName;

	unsigned int CurFrame;

	std::vector<unsigned int> Frames;

	float FrameTime;

	float Inter; // 0.1f

	bool Loop;
	// 아틀라스 애니메이션

	class GameEngineTextureRenderer* Renderer;

public:
	FrameAnimation_DESC()
		: Loop(false)
		, Inter(0.1f)
		, CurFrame(-1)
		, FrameTime(0.0f)
	{

	}

	FrameAnimation_DESC(const std::string _TextureName, unsigned int _Start, unsigned int _End, float _Inter, bool _Loop = true)
		: TextureName(_TextureName)
		, Loop(_Loop)
		, Inter(_Inter)
		, CurFrame(0)
		, FrameTime(0.0f)
	{
		if (_Start <= _End) {
			for (unsigned int i = _Start; i <= _End; i++) {
				Frames.push_back(i);
			}
		} else {
			for (int i = _Start; i >= (int)_End; i--) {
				Frames.push_back(i);
			}
		}

	}

	FrameAnimation_DESC(const std::string _TextureName, const std::vector<unsigned int>& _Frames, float _Inter, bool _Loop = true)
		: TextureName(_TextureName)
		, Loop(_Loop)
		, Inter(_Inter)
		, Frames(_Frames)
		, FrameTime(0.0f)
	{
		
	}


	FrameAnimation_DESC(const std::string _TextureName, float _Inter, bool _Loop = true)
		: TextureName(_TextureName)
		, Loop(_Loop)
		, Inter(_Inter)
		, CurFrame(0)
		, FrameTime(0.0f)
	{

	}
};

enum ChangeAnimationOption {
	FORCE = 1,
	RESTART = 2,
};

class GameEngineFolderTexture;
class GameEngineTextureRenderer;
class FrameAnimation : public GameEngineNameObject
{
	friend GameEngineTextureRenderer;

public:
	FrameAnimation_DESC Info;

	GameEngineTextureRenderer* ParentRenderer;
	GameEngineTexture* Texture;
	GameEngineFolderTexture* FolderTexture;

	bool Pause;
	bool bOnceStart;
	bool bOnceEnd; // 애니메이션 끝났을 때
	std::function<void(const FrameAnimation_DESC&)> Frame;
	std::function<void(const FrameAnimation_DESC&)> End;
	std::function<void(const FrameAnimation_DESC&)> Start;
	std::function<void(const FrameAnimation_DESC&, float)> Time;

	DWORD Flag;

	void PauseSwtich();

	void Reset();

	void Update(float _DeltaTime);

public:
	FrameAnimation() 
		: bOnceStart(true)
		, bOnceEnd(false)
		, Pause(false)
		, Flag(0)
	{

	}
};

// 설명 :
class GameEngineTextureRenderer : public GameEngineDefaultRenderer
{
	friend FrameAnimation;

public:
	// constrcuter destructer
	GameEngineTextureRenderer();
	~GameEngineTextureRenderer();

	// delete Function
	GameEngineTextureRenderer(const GameEngineTextureRenderer& _Other) = delete;
	GameEngineTextureRenderer(GameEngineTextureRenderer&& _Other) noexcept = delete;
	GameEngineTextureRenderer& operator=(const GameEngineTextureRenderer& _Other) = delete;
	GameEngineTextureRenderer& operator=(GameEngineTextureRenderer&& _Other) noexcept = delete;

	void SetSamplingModePoint();
	void SetSamplingModeLiner();

	void SetScaleMode(SCALEMODE _ScaleMode)
	{
		ScaleMode = _ScaleMode;
	}

	void SetScaleRatio(float _Scale) 
	{
		ScaleRatio = _Scale;
	}

	float GetScaleRatio() 
	{
		return ScaleRatio;
	}

	bool IsCurAnimation()
	{
		if (nullptr == CurAni)
		{
			return false;
		}
		return true;
	}

	void SetTexture(GameEngineTexture* _Texture);

	void SetTexture(const std::string& _Name);

	void SetFrame(UINT _Index);

	GameEngineTexture* GetCurTexture();

	void SetTexture(const std::string& _Name, UINT _Index);

	void SetPivot();

	void SetPivot(PIVOTMODE _Mode);

	void SetPivotToVector(const float4& _Value);

	void SetTexture(GameEngineTexture* _Texture, UINT _Index);

	void SetFolderTextureToIndex(const std::string& _Text, UINT _Index);

	void CreateFrameAnimationFolder(const std::string& _AnimationName, const FrameAnimation_DESC& _Desc);

	void CreateFrameAnimationCutTexture(const std::string& _AnimationName, const FrameAnimation_DESC& _Desc);
	void ChangeFrameAnimation(const std::string& _AnimationName, DWORD _Flag = 0);

	void ScaleToTexture();

	void ScaleToCutTexture(int _Index = 0);

	void CurAnimationPauseSwitch();

	void CurAnimationPauseOn();

	void CurAnimationPauseOff();

	bool IsCurAnimationPause();

	void CurAnimationReset();

	void CurAnimationSetStartPivotFrame(int SetFrame);

	bool CurAnimationEnd();

	int CurAnimationFrameIndex();

	PixelData& GetPixelData()
	{
		return PixelDataInst;
	}


	// 애니메이션 바인드
	// 시작 프레임에 들어온다.
	void AnimationBindStart(const std::string& _AnimationName, std::function<void(const FrameAnimation_DESC&)> _Function)
	{
		std::string Name = GameEngineString::ToUpperReturn(_AnimationName);

		if (FrameAni->end() == FrameAni->find(Name))
		{
			MsgBoxAssert("존재하지 않는 애니메이션으로 체인지 하려고 했습니다.");
			return;
		}

		(*FrameAni)[Name].Start = _Function;
	}
	// 끝나는 프레임에 들어온다
	void AnimationBindEnd(const std::string& _AnimationName, std::function<void(const FrameAnimation_DESC&)> _Function)
	{
		std::string Name = GameEngineString::ToUpperReturn(_AnimationName);

		if (FrameAni->end() == FrameAni->find(Name))
		{
			MsgBoxAssert("존재하지 않는 애니메이션으로 체인지 하려고 했습니다.");
			return;
		}

		(*FrameAni)[Name].End = _Function;
	}
	// 프레임이 바뀔때마다 들어온다
	void AnimationBindFrame(const std::string& _AnimationName, std::function<void(const FrameAnimation_DESC&)> _Function)
	{
		std::string Name = GameEngineString::ToUpperReturn(_AnimationName);

		if (FrameAni->end() == FrameAni->find(Name))
		{
			MsgBoxAssert("존재하지 않는 애니메이션으로 체인지 하려고 했습니다.");
			return;
		}

		(*FrameAni)[Name].Frame = _Function;
	}
	// Update
	void AnimationBindTime(const std::string& _AnimationName, std::function<void(const FrameAnimation_DESC&, float)> _Function)
	{
		std::string Name = GameEngineString::ToUpperReturn(_AnimationName);

		if (FrameAni->end() == FrameAni->find(Name))
		{
			MsgBoxAssert("존재하지 않는 애니메이션으로 체인지 하려고 했습니다.");
			return;
		}

		(*FrameAni)[Name].Time = _Function;
	}

	std::shared_ptr<std::map<std::string, FrameAnimation>> GetFrameAni() const {
		return FrameAni;
	}

	void SetFrameAni(std::shared_ptr<std::map<std::string, FrameAnimation>> _FrameAni) {
		FrameAni.reset();

		Copied = true;

		FrameAni = _FrameAni;
	}

	const FrameAnimation* GetCurAni() {
		return CurAni;
	}

	const std::string& GetTexName() {
		return TexName;
	}

protected:
	void Start() override;

	void SetTextureRendererSetting();

	void Update(float _Delta) override;

private:
	bool Copied = false;

	PIVOTMODE PivotMode;
	SCALEMODE ScaleMode;
	float ScaleRatio;

	std::string TexName;
	GameEngineTexture* CurTex;

	PixelData PixelDataInst;
	AtlasData AtlasDataInst;

	std::shared_ptr<std::map<std::string, FrameAnimation>> FrameAni;
	FrameAnimation* CurAni;

	void FrameDataReset();
};

