#pragma once

// std and direct
#include <string>

#include <map>
#include <unordered_map>
#include <list>
#include <set>
#include <vector>
#include <chrono>
#include <queue>
#include <thread>
#include <format>

#include <Windows.h>

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXPackedVector.h>

#include <DirectXCollision.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

// base
#include <GameEngineBase/GameEngineDebug.h>
#include <GameEngineBase/GameEngineInput.h>

// core
#include <GameEngineCore/CoreMinimal.h>
#include <GameEngineCore/GEngine.h>
#include <GameEngineCore/GameEngineDefaultRenderer.h>
#include <GameEngineCore/GameEngineRenderingPipeLine.h>
#include <GameEngineCore/GameEngineVertexShader.h>
#include <GameEngineCore/GameEngineConstantBuffer.h>
#include <GameEngineCore/GameEngineDevice.h>
#include <GameEngineCore/GameEngineTextureRenderer.h>
#include <GameEngineCore/GameEngineActorInfo.h>
#include <GameEngineCore/GameEngineLevel.h>
#include <GameEngineCore/GameEngineCollision.h>
#include <GameEngineCore/GameEngineFont.h>
#include <GameEngineCore/GameEngineFontRenderer.h>
#include <GameEngineCore/GameEngineCoreDebug.h>

#include <GameEngineCore/GameEngineGlobalVariable.h>
#include "GlobalContentsValue.h"

#include "TileMapActor.h"

#define ROOM(_Idx) 10000 + _Idx

using namespace std;

template <class T>
struct Node {
	T Data;
	Node* NextNode;
};

template <class T>
class LinkedList {
private:
	int Size;
	Node<T>* Head, * Tail;
public:
	LinkedList() {
		Size = 0;
		Head = nullptr;
		Tail = nullptr;
	}

	inline void AddFrontNode(T _Data) {
		Node<T>* Temp = new Node<T>;
		Temp->Data = _Data;

		if (Head == nullptr) {
			Head = Temp;
			Tail = Temp;
		} else {
			Temp->NextNode = Head;
			Head = Temp;
		}
		Size++;
	}
	inline Node<T>* AddNode(T _Data) {
		Node<T>* Temp = new Node<T>();

		Temp->Data = _Data;
		Temp->NextNode = nullptr;

		if (Head == nullptr) {
			Head = Temp;
			Tail = Temp;
		} else {
			Tail->NextNode = Temp;
			Tail = Temp;
		}
		Size++;

		return Temp;
	}

	inline void InsertNode(Node<T>* _PrevNode, T _Data) {
		Node<T>* Temp = new Node<T>();
		Temp->Data = _Data;
		Temp->NextNode = _PrevNode->NextNode;
		_PrevNode->NextNode = Temp;
		Size++;
	}

	inline void DeleteNode(Node<T>* _PrevNode) {
		Node<T>* Temp = _PrevNode->NextNode;
		_PrevNode->NextNode = Temp->NextNode;
		delete Temp;
		Size--;
	}

	Node<T>* GetHead() {
		return Head;
	}

	Node<T>* GetTail() {
		return Tail;
	}

	int GetSize() {
		return this->Size;
	}
};

struct ObjectBatchInfo {
	ObjectBatchInfo() {}
	ObjectBatchInfo(const ObjectBatchInfo& _Info) {
		this->Index = _Info.Index;
		this->Name = _Info.Name;
		this->Renderer = _Info.Renderer;
		this->Texture = _Info.Texture;
	}
	int Index;
	string Name;
	GameEngineTextureRenderer* Renderer;
	ID3D11ShaderResourceView* Texture;
};

class CONTAINER_DELETE {
public:
	template<typename T>
	void operator()(const T* ptr) const {
		if (ptr != nullptr) {
			delete ptr;
		}
	}
};

#define SAFE_DELETE(p) { if (p) { delete p; p = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] p; p = nullptr; } }