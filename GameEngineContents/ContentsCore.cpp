#include "PreCompile.h"
#include "ContentsCore.h"
#include "GameEngineContents/LoginLevel.h"
#include "GameEngineContents/PlayLevel.h"
#include "GameEngineContents/MapEditorLevel.h"
#include "MetaLevel.h"

#include <GameEngineBase/magic_enum.hpp>


#pragma comment(lib, "GameEngineBase.lib")

ContentsCore::ContentsCore() 
	: GameEngineCore()
{
}

ContentsCore::~ContentsCore() 
{
}

void ContentsCore::Start()
{

	//{
	//	// 1, 1, 1, 1 * ���� + 0, 0��, * ����

	//	D3D11_BLEND_DESC Desc = { 0 };

	//	Desc.AlphaToCoverageEnable = FALSE;
	//	Desc.IndependentBlendEnable = FALSE;
	//	Desc.RenderTarget[0].BlendEnable = true;
	//	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_MAX;
	//	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	//	Desc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	//	// Desc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_DEST_COLOR;
	//	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	//	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	//	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	//	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//	//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//	//blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//	//blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//	//blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//	//blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//	//blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	//	GameEngineBlend::Create("TransparentBlend", Desc);
	//}


	// string ����ȭ�� ���ѳ༮.
	std::string_view Value = magic_enum::enum_name(OBJECTORDER::Player);
	std::string Name = Value.data();

	{
		GameEngineDirectory Dir;
		Dir.MoveParentToExitsChildDirectory("ContentsResources");
		Dir.Move("ContentsResources");
		Dir.Move("Texture");

		// std::vector<GameEngineDirectory> DirList = Dir.GetRecursiveAllDirectory();

		std::vector<GameEngineFile> Shaders = Dir.GetAllFile();

		for (size_t i = 0; i < Shaders.size(); i++)
		{
			GameEngineTexture::Load(Shaders[i].GetFullPath());
		}
	}

	GameEngineFont::Load("�ü�");

	//Dir.Move("BlackSet");
	//GameEngineFolderTexture::Load(Dir.GetFullPath());

	// �̰� ����� �մϴ�.
	// GameEngineTexture::Cut("Boss_Left.bmp", 5, 7);


	if (false == GameEngineInput::GetInst()->IsKey("LevelChangeKey"))
	{
		GameEngineInput::GetInst()->CreateKey("LevelChangeKey", 'P');
	}

	// ���ҽ��� �ε��ϴµ�.

	// RTTI �� Ÿ�� Ÿ�� �������̼�
	CreateLevel<LoginLevel>("Login");
	CreateLevel<PlayLevel>("Play");
	CreateLevel<MapEditorLevel>("MapEditor");
	CreateLevel<MetaLevel>("MetaEditor");
	
	ChangeLevel("Play");

	// ChangeLevel("Login");

	GameEngineGUI::CreateGUIWindow<GameEngineStatusWindow>("EngineStatus", nullptr);

}

void ContentsCore::Update(float _DeltaTime)
{
	// ������ ����Ǿ���.
	// ������ ��� �޼���â�� �������Ѵ�.
	
	
}

void ContentsCore::End()
{
	int a = 0;
}