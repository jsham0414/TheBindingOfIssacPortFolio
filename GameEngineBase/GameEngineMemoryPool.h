#pragma once
#include <unordered_set>
#include <list>

#ifdef max
#undef max
#endif

// 오브젝트 타입과 할당 크기
template <typename T, size_t UnitSize>
class GameEngineMemoryPool {
public:
	using byte = char;
	using Object = T;

	static constexpr int null() { return 0; }

	// new는 void* 포인터 반환
	static void* operator new(size_t _allocMemoryLength) {
		SupplementBlock();
		return AllocateMemory(_allocMemoryLength);
	}

	static void operator delete(void* _exhaustedMemory) {
		DeallocateMemory(_exhaustedMemory);
	}

private:
	class GameEngineMemoryBlock {
	public:
		void* Allocate(size_t _allocMemoryLength) noexcept {
			byte* available_memory = FreePointer;
			// 다음 메모리의 주소를 가져온다.
			FreePointer = *reinterpret_cast<byte**>(FreePointer);
			
			Count++;

			return available_memory;
		}

		void Deallocate(void* _exhaustedMemory) noexcept {
			// 반환할 메모리 앞 4바이트는 다시 다음 메모리를 가르킨다.
			*reinterpret_cast<byte**>(_exhaustedMemory) = FreePointer;

			// 반환될 메모리 주소로 이동
			FreePointer = static_cast<byte*>(_exhaustedMemory);

			Count--;
		}

		GameEngineMemoryBlock() :
			MemorySize(sizeof(Object) * UnitSize + sizeof(void*)),
			FreePointer(nullptr), BlockAddress(nullptr), Count(0) {

			AllocateBlock();
			WriteEmptyMemorySequence();
		}

		~GameEngineMemoryBlock() {
			delete[] BlockAddress;
		}

		const bool Full() const noexcept {
			return nullptr == FreePointer;
		}

		const bool Empty() const noexcept {
			return 0 == Count;
		}

		const bool operator<(const GameEngineMemoryBlock& rhs) const noexcept {
			return BlockAddress < rhs.BlockAddress;
		}

		/*

		int a = 2;
		int& b = a;
		b = 3;

		int& ref = 5; // Error -> l-value reference가 int형인 r-value는 참조할 수 없다.

		// 상수 2가 ref라는 별명을 받고 l-value가 됨
		int&& ref = 2;
		ref = 3;

		*/

		const bool operator<(GameEngineMemoryBlock&& rhs) const noexcept {
			return BlockAddress < rhs.BlockAddress;
		}

		const bool operator==(const GameEngineMemoryBlock& rhs) const noexcept {

			return BlockAddress == rhs.BlockAddress;
		}

		// 주소의 범위를 확인한다.
		const bool IsInclude(void* _findingMemory) const noexcept {
			unsigned long long a = PreventUnderflow(reinterpret_cast<unsigned long long>(_findingMemory), MemorySize);
			unsigned long long b = reinterpret_cast<unsigned long long>(BlockAddress);

			return a < b && BlockAddress <= _findingMemory;
		}

	private:
		const unsigned long long PreventUnderflow(const unsigned long long lhs, const unsigned long long rhs) const noexcept {
			return std::max(lhs - rhs, 0ull);
		}

		// 블럭의 메모리를 할당한다.
		void AllocateBlock() {
			FreePointer = new byte[MemorySize];
			BlockAddress = FreePointer;
		}

		// 각각 앞 4바이트에 다음 메모리 주소를 넣어준다. 
		void WriteEmptyMemorySequence() noexcept {
			byte** cur = reinterpret_cast<byte**>(FreePointer);
			byte* next = FreePointer;

			for (int i = 1; i < UnitSize; i++) {
				next += sizeof(Object);
				*cur = next;
				cur = reinterpret_cast<byte**>(next);
			}
			*cur = nullptr;
		}

		byte* BlockAddress;		// 메모리풀의 시작점
		byte* FreePointer;		// 바로 할당 가능한 주소
		size_t MemorySize;		// 오브젝트의 크기 * 블럭에 들어갈 수 있는 오브젝트의 방 개수 + 널포인터
		size_t Count;				// 블록에 있는 오브젝트의 개수
	};

	// 사용 가능한 블럭이 없을 때 보충해준다.
	static void SupplementBlock() {
		if (AvailableBlocks.empty()) {
			auto block = new GameEngineMemoryBlock;
			MemoryBlocks.insert(block);
			AvailableBlocks.push_back(block);
		}
	}

	// 사용 가능한 블럭에서 메모리 주소를 가져오고 블럭이 가득 찼는지 체크한다.
	static auto AllocateMemory(size_t _MemoryLength) {
		auto block{ AvailableBlocks.front() };
		auto result{ block->Allocate(_MemoryLength) };
		if (block->Full()) {
			AvailableBlocks.pop_front();
		}

		return result;
	}

	// 메모리를 반납하고 할당 가능한 상태로 만든다.
	static void DeallocateMemory(void* _exhaustedMemory) {
		for (auto block : MemoryBlocks) {
			if (block->IsInclude(_exhaustedMemory)) {
				if (block->Full()) {
					AvailableBlocks.push_back(block);
				}

				block->Deallocate(_exhaustedMemory);

				if (block->Empty()) {
					delete block;
					MemoryBlocks.erase(block);
					AvailableBlocks.erase(std::find(AvailableBlocks.begin(), AvailableBlocks.end(), block));
				}

				return;
			}
		}
	}

	// CRTP 패턴 정의
	static std::unordered_set<GameEngineMemoryBlock*> MemoryBlocks;
	static std::list<GameEngineMemoryBlock*> AvailableBlocks;

protected:
	~GameEngineMemoryPool() {}
};

template<typename T, size_t UnitSize>
std::unordered_set<typename GameEngineMemoryPool<T, UnitSize>::GameEngineMemoryBlock*> GameEngineMemoryPool<T, UnitSize>::MemoryBlocks;
template<typename T, size_t UnitSize>
std::list<typename GameEngineMemoryPool<T, UnitSize>::GameEngineMemoryBlock*> GameEngineMemoryPool<T, UnitSize>::AvailableBlocks;
