#ifndef CAM360_LIBCAM_CLIENT_MEMORYPOOL_H_
#define CAM360_LIBCAM_CLIENT_MEMORYPOOL_H_
#include <atomic>
#include <list>
#include <mutex>
#include "../type.h"

#if defined(_WINDOWS)
#include <Windows.h>
#else

#endif  // _WINDOWS

typedef struct tag_MEMORY_BLOCK{
    DWORD  dwBufferSize;
    LPBYTE lpbyBuffer;
    int    iDataLength;
    int    iDataOffset;
}MEMORY_BLOCK, *LPMEMORY_BLOCK;

bool MemoryBlock_Construct(DWORD  dwBufferSize, LPMEMORY_BLOCK *ppBlock);
void MemoryBlock_Destruct(LPMEMORY_BLOCK pBlock);
int  MemoryBlock_Cat(LPMEMORY_BLOCK pBlock, const LPBYTE pData, int length);
int  MemoryBlock_Fetch(LPMEMORY_BLOCK pBlock, LPBYTE pData, int length);
int  MemoryBlock_Peek(LPMEMORY_BLOCK pBlock, LPBYTE pData, int length);

class CMemoryPool;
class CMemoryItem;

class CMemoryPool{
public:
    CMemoryPool(int buff_size = 4096, int buff_num = 10);
    ~CMemoryPool();
    CMemoryItem* Construct();
private:
    const int kBufferSize;
    std::recursive_mutex       m_MemoryBlock_list_mutex;
    std::list<LPMEMORY_BLOCK>  m_MemoryBlock_list;
    std::atomic_llong          m_Item_Count;

    void Destruct(CMemoryItem* pItem);
    bool GetMemoryBlock(LPMEMORY_BLOCK *block);
    bool FreeMemoryBlock(LPMEMORY_BLOCK block);

    friend class CMemoryItem;
};

class CMemoryItem{
public:
    ~CMemoryItem();
    int  Cat(const LPBYTE pData, int length);
    int  Cat(CMemoryItem* other);
    int  Fetch(LPBYTE pData, int length);
    int  Peek(LPBYTE pData, int length);
    int  Reduce(int length);
    void Reset(int begin = 0, int end = 0);
    int  Size();
private:
    CMemoryItem(CMemoryPool* pool) :kMemoryPool(pool){ ; };
    std::recursive_mutex       m_MemoryBlock_list_mutex;
    std::list<LPMEMORY_BLOCK> m_MemoryBlock_list;
    CMemoryPool * kMemoryPool;

    friend class CMemoryPool;
};

#endif
