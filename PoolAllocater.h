#pragma once
#include <vector>

using namespace std;

// class Tを最大MAXSIZE個確保可能なPoolAllocatorを実装してください
template<class T, size_t MAXSIZE> class PoolAllocator
{
public:
	// コンストラクタ
	PoolAllocator() : m_pool{ new T[MAXSIZE] } {
		for (int i = 0; i < MAXSIZE; i++)
		{
			m_freelist.push_back(&m_pool[i]);
		}
	}

	// デストラクタ
	~PoolAllocator() {
		delete[] m_pool;
	}

	// 確保できない場合はnullptrを返す事。
	T* Alloc() {
		if (m_freelist.empty()) return nullptr;

		T* addr = m_freelist.back();
		m_freelist.pop_back();

		return addr;
	}

	// Free(nullptr)で誤動作しないようにする事。
	void Free(T* addr) {
		if (addr == nullptr) return;
		m_freelist.push_back(addr);
		addr = nullptr;
	}

private:
	T* m_pool;
	vector<T*> m_freelist;
};
