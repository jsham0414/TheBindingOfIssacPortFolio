#pragma once
#include <GameEngineBase/GameEngineNameObject.h>
#include "GameEngineUpdateObject.h"
#include <GameEngineBase/GameEngineString.h>
#include <list>
#include <map>
#include <queue>
#include <unordered_map>

enum class CAMERAORDER
{
	MAINCAMERA,
	USER0,
	USER1,
	USER2,
	USER3,
	USER4,
	USER5,
	USER6,
	USER7,
	UICAMERA,
};

#define STR_SWITCH_BEGIN(key) {\
　　static unordered_map< string, int > s_hm;\
　　static bool s_bInit = false;\
　　bool bLoop = true;\
　　while(bLoop) {\
　　　　int nKey = -1;\
　　　　if(s_bInit) { nKey=s_hm[key]; bLoop = false; }\
　　　　switch(nKey) {\
　　　　　　case -1: {

#define CASE(token)  } case __LINE__: if(!s_bInit) s_hm[token] = __LINE__; else {
#define DEFAULT()    } case 0: default: if(s_bInit) {

#define STR_SWITCH_END()\
　　　　　　}\
　　　　}\
　　　　if(!s_bInit) s_bInit=true;\
　　}\
}

// 설명 : 화면(타이틀 화면, 플레이 화면, 인벤토리 화면)
class GameEngineCore;
class GameEngineActor;
class GameEngineCamera;
class GameEngineRenderer;
class GameEngineTransform;
class GameEngineCollision;
class GameEngineCameraActor;
class MapManager;
class UIManager;
class GameEngineLevel :
	public GameEngineNameObject ,
	public GameEngineUpdateObject
{
	friend GameEngineCore;
	friend GameEngineActor;
	friend GameEngineCamera;
	friend GameEngineRenderer;
	friend GameEngineCollision;
	// 레벨이 현재까지 얼마나 켜져있었는지 시간을 잴수 있게 한다.

public:
	// constrcuter destructer
	GameEngineLevel();
	virtual ~GameEngineLevel() = 0;

	// delete Function
	GameEngineLevel(const GameEngineLevel& _Other) = delete;
	GameEngineLevel(GameEngineLevel&& _Other) noexcept = delete;
	GameEngineLevel& operator=(const GameEngineLevel& _Other) = delete;
	GameEngineLevel& operator=(GameEngineLevel&& _Other) noexcept = delete;

	GameEngineCamera* GetMainCamera() 
	{
		return Cameras[static_cast<int>(CAMERAORDER::MAINCAMERA)];
	}

	GameEngineCamera* GetUICamera()
	{
		return Cameras[static_cast<int>(CAMERAORDER::UICAMERA)];
	}

	GameEngineCameraActor* GetMainCameraActor();

	GameEngineTransform& GetMainCameraActorTransform();

	GameEngineCameraActor* GetUICameraActor();

	GameEngineTransform& GetUICameraActorTransform();

	//template<typename ReturnType, typename ActorType, typename GroupIndexType>
	//ReturnType* CreateActor(GroupIndexType _ObjectGroupIndex)
	//{
	//	return CreateActor<ActorType>(static_cast<int>(_ObjectGroupIndex));
	//}

	template<typename ActorType, typename GroupIndexType>
	ActorType* CreateActor(GroupIndexType _ObjectGroupIndex)
	{
		return CreateActor<ActorType>(static_cast<int>(_ObjectGroupIndex));
	}

	template<typename ActorType>
	ActorType* CreateActor(const std::string _Name, int _ObjectGroupIndex = 0)
	{
		CreateActor(_ObjectGroupIndex);
	}

	template<typename ActorType>
	ActorType* CreateActor(int _ObjectGroupIndex = 0) {
		GameEngineActor* NewActor = new ActorType();
		NewActor->SetLevel(this);
		NewActor->SetOrder(_ObjectGroupIndex);
		NewActor->Start();

		PushActor(NewActor, _ObjectGroupIndex);

		return dynamic_cast<ActorType*>(NewActor);
	}


	template<typename ActorType, typename GroupIndexType>
	ActorType* CreateDummy(GroupIndexType _ObjectGroupIndex) {
		return CreateDummy<ActorType>(static_cast<int>(_ObjectGroupIndex));
	}

	template<typename ActorType>
	ActorType* CreateDummy(int _ObjectGroupIndex = 4) {
		GameEngineActor* NewActor = new ActorType();
		NewActor->SetLevel(this);
		NewActor->SetOrder(_ObjectGroupIndex);
		NewActor->Start();
		NewActor->Off();

		PushActor(NewActor, _ObjectGroupIndex);

		return dynamic_cast<ActorType*>(NewActor);
	}


	template<typename GroupIndexType>
	std::list<GameEngineActor*> GetGroup(GroupIndexType _ObjectGroupIndex)
	{
		return AllActors[static_cast<int>(_ObjectGroupIndex)];
	}

	std::list<GameEngineActor*> GetGroup(int _ObjectGroupIndex)
	{
		return AllActors[_ObjectGroupIndex];
	}

	template<typename ObjectType, typename GroupIndexType>
	std::list<ObjectType*> GetConvertToGroup(GroupIndexType _ObjectGroupIndex)
	{
		return GetConvertToGroup<ObjectType>(static_cast<int>(_ObjectGroupIndex));
	}


	template<typename ObjectType>
	std::list<ObjectType*> GetConvertToGroup(int _ObjectGroupIndex)
	{
		std::list<ObjectType*> Result;
		for (GameEngineActor* Object : AllActors[_ObjectGroupIndex])
		{
			Result.push_back(dynamic_cast<ObjectType*>(Object));
		}

		return Result;
	}

	virtual bool PreLoading() { return true; }

	UIManager* GetUIManager() {
		return UIManagerInstance;
	}

	MapManager* GetMapManager() {
		return MapManagerInstance;
	}

	void AllClear();

	const std::list<GameEngineActor*>& GetActors(int _Order) {
		return AllActors[_Order];
	}

	bool GetPause() {
		return Pause;
	}

	void SetPause(bool _Pause) {
		Pause = _Pause;
	}

	bool CompareName(std::string _Name) {
		return this->GetNameConstPtr() == GameEngineString::ToUpperReturn(_Name);
	}

	virtual bool LoadTexture() { return true; }
protected:
	UIManager* UIManagerInstance;
	MapManager* MapManagerInstance;
	bool Pause;
private:
	void PushActor(GameEngineActor* _Actor, int _ObjectGroupIndex);

	void ActorLevelStartEvent();

	void ActorLevelEndEvent();

	void ActorUpdate(float _DelataTime);
	
	void ActorTransformUpdate(float _DeltaTime);

	void LevelUpdate(float DeltaTime);

	void RemoveActor(GameEngineActor* _Actor);

	void OverChildMove(GameEngineLevel* _NextLevel);

	void PushCamera(GameEngineCamera* _Camera, CAMERAORDER _Order) 
	{
		PushCamera(_Camera, static_cast<int>(_Order));
	}

	void PushRendererToMainCamera(GameEngineRenderer* _Renderer)
	{
		PushRenderer(_Renderer, static_cast<int>(CAMERAORDER::MAINCAMERA));
	}

	void PushRendererToUICamera(GameEngineRenderer* _Renderer)
	{
		PushRenderer(_Renderer, static_cast<int>(CAMERAORDER::UICAMERA));
	}

	void PushRenderer(GameEngineRenderer* _Renderer, CAMERAORDER _Order) 
	{
		PushRenderer(_Renderer, static_cast<int>(_Order));
	}

	void PushCamera(GameEngineCamera* _Camera, int _CameraOrder);

	void PushRenderer(GameEngineRenderer* _Renderer, int _CameraOrder);

	void PushCollision(GameEngineCollision* _Collision, int _Order);

	void Render(float _DeltaTime);

	void Release(float _DeltaTime);

private:

	std::map<int, std::list<GameEngineActor*>> AllActors;
	std::unordered_map<std::string, std::queue<GameEngineActor*>> ObjectPools;

	std::list<GameEngineUpdateObject*> DeleteObject;

	std::vector<GameEngineCamera*> Cameras;

	std::map<int, std::list<GameEngineCollision*>> AllCollisions;
};

