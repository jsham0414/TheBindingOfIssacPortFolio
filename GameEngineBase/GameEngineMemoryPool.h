#pragma once
#include <unordered_set>
#include <list>

#ifdef max
#undef max
#endif

// ������Ʈ Ÿ�԰� �Ҵ� ũ��
template <typename T, size_t UnitSize>
class GameEngineMemoryPool {
public:
	using byte = char;
	using Object = T;

	static constexpr int null() { return 0; }

	// new�� void* ������ ��ȯ
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
			// ���� �޸��� �ּҸ� �����´�.
			FreePointer = *reinterpret_cast<byte**>(FreePointer);
			
			Count++;

			return available_memory;
		}

		void Deallocate(void* _exhaustedMemory) noexcept {
			// ��ȯ�� �޸� �� 4����Ʈ�� �ٽ� ���� �޸𸮸� ����Ų��.
			*reinterpret_cast<byte**>(_exhaustedMemory) = FreePointer;

			// ��ȯ�� �޸� �ּҷ� �̵�
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

		int& ref = 5; // Error -> l-value reference�� int���� r-value�� ������ �� ����.

		// ��� 2�� ref��� ������ �ް� l-value�� ��
		int&& ref = 2;
		ref = 3;

		*/

		const bool operator<(GameEngineMemoryBlock&& rhs) const noexcept {
			return BlockAddress < rhs.BlockAddress;
		}

		const bool operator==(const GameEngineMemoryBlock& rhs) const noexcept {

			return BlockAddress == rhs.BlockAddress;
		}

		// �ּ��� ������ Ȯ���Ѵ�.
		const bool IsInclude(void* _findingMemory) const noexcept {
			unsigned long long a = PreventUnderflow(reinterpret_cast<unsigned long long>(_findingMemory), MemorySize);
			unsigned long long b = reinterpret_cast<unsigned long long>(BlockAddress);

			return a < b && BlockAddress <= _findingMemory;
		}

	private:
		const unsigned long long PreventUnderflow(const unsigned long long lhs, const unsigned long long rhs) const noexcept {
			return std::max(lhs - rhs, 0ull);
		}

		// ���� �޸𸮸� �Ҵ��Ѵ�.
		void AllocateBlock() {
			FreePointer = new byte[MemorySize];
			BlockAddress = FreePointer;
		}

		// ���� �� 4����Ʈ�� ���� �޸� �ּҸ� �־��ش�. 
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

		byte* BlockAddress;		// �޸�Ǯ�� ������
		byte* FreePointer;		// �ٷ� �Ҵ� ������ �ּ�
		size_t MemorySize;		// ������Ʈ�� ũ�� * ���� �� �� �ִ� ������Ʈ�� �� ���� + ��������
		size_t Count;				// ��Ͽ� �ִ� ������Ʈ�� ����
	};

	// ��� ������ ���� ���� �� �������ش�.
	static void SupplementBlock() {
		if (AvailableBlocks.empty()) {
			auto block = new GameEngineMemoryBlock;
			MemoryBlocks.insert(block);
			AvailableBlocks.push_back(block);
		}
	}

	// ��� ������ ������ �޸� �ּҸ� �������� ���� ���� á���� üũ�Ѵ�.
	static auto AllocateMemory(size_t _MemoryLength) {
		auto block{ AvailableBlocks.front() };
		auto result{ block->Allocate(_MemoryLength) };
		if (block->Full()) {
			AvailableBlocks.pop_front();
		}

		return result;
	}

	// �޸𸮸� �ݳ��ϰ� �Ҵ� ������ ���·� �����.
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

	// CRTP ���� ����
	static std::unordered_set<GameEngineMemoryBlock*> MemoryBlocks;
	static std::list<GameEngineMemoryBlock*> AvailableBlocks;

protected:
	~GameEngineMemoryPool() {}
};

template<typename T, size_t UnitSize>
std::unordered_set<typename GameEngineMemoryPool<T, UnitSize>::GameEngineMemoryBlock*> GameEngineMemoryPool<T, UnitSize>::MemoryBlocks;
template<typename T, size_t UnitSize>
std::list<typename GameEngineMemoryPool<T, UnitSize>::GameEngineMemoryBlock*> GameEngineMemoryPool<T, UnitSize>::AvailableBlocks;
