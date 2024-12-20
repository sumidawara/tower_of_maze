#pragma once
#include <vector>

using namespace std;

// class T���ő�MAXSIZE�m�ۉ\��PoolAllocator���������Ă�������
template<class T, size_t MAXSIZE> class PoolAllocator
{
public:
	// �R���X�g���N�^
	PoolAllocator() : m_pool{ new T[MAXSIZE] } {
		for (int i = 0; i < MAXSIZE; i++)
		{
			m_freelist.push_back(&m_pool[i]);
		}
	}

	// �f�X�g���N�^
	~PoolAllocator() {
		delete[] m_pool;
	}

	// �m�ۂł��Ȃ��ꍇ��nullptr��Ԃ����B
	T* Alloc() {
		if (m_freelist.empty()) return nullptr;

		T* addr = m_freelist.back();
		m_freelist.pop_back();

		return addr;
	}

	// Free(nullptr)�Ō듮�삵�Ȃ��悤�ɂ��鎖�B
	void Free(T* addr) {
		if (addr == nullptr) return;
		m_freelist.push_back(addr);
		addr = nullptr;
	}

private:
	T* m_pool;
	vector<T*> m_freelist;
};
