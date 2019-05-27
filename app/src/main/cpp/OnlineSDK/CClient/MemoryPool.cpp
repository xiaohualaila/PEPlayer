#include "MemoryPool.h"
#include <algorithm>
#include <numeric>

////////////////////////////////////////////////////////////////////////////////

bool MemoryBlock_Construct(DWORD  dwBufferSize, LPMEMORY_BLOCK *ppBlock)
{
    LPMEMORY_BLOCK tmp = NULL;
    try{
        tmp = new MEMORY_BLOCK;
        tmp->dwBufferSize = dwBufferSize;
        tmp->lpbyBuffer = NULL;
        tmp->lpbyBuffer = new BYTE[dwBufferSize];
        tmp->iDataOffset = 0;
        tmp->iDataLength = 0;
    }
    catch (std::bad_alloc){
        if (tmp){
            delete tmp;
        }
        return false;
    }
    *ppBlock = tmp;
    return true;
}

void MemoryBlock_Destruct(LPMEMORY_BLOCK pBlock)
{
    delete[] pBlock->lpbyBuffer;
    delete pBlock;
}

int MemoryBlock_Cat(LPMEMORY_BLOCK pBlock, const LPBYTE pData, int length)
{
    if (pBlock->iDataOffset > 0){
        if (pBlock->iDataLength > 0){
            LPBYTE tmp = new BYTE[pBlock->iDataLength];
            memcpy(tmp, pBlock->lpbyBuffer + pBlock->iDataOffset, pBlock->iDataLength);
            memcpy(pBlock->lpbyBuffer, tmp, pBlock->iDataLength);
            delete[] tmp;
        }
        pBlock->iDataOffset = 0;
    }
    int write_size = std::min((int)pBlock->dwBufferSize - pBlock->iDataLength, length);
    if (write_size > 0){
        memcpy(pBlock->lpbyBuffer + pBlock->iDataLength, pData, write_size);
        pBlock->iDataLength += write_size;
    }
    return write_size;
}

int MemoryBlock_Fetch(LPMEMORY_BLOCK pBlock, LPBYTE pData, int length)
{
    int read_size = std::min(pBlock->iDataLength, length);
    if (read_size > 0){
        memcpy(pData, pBlock->lpbyBuffer + pBlock->iDataOffset, read_size);
        pBlock->iDataOffset += read_size;
        pBlock->iDataLength -= read_size;
    }
    return read_size;
}

int MemoryBlock_Peek(LPMEMORY_BLOCK pBlock, LPBYTE pData, int length)
{
    int read_size = std::min(pBlock->iDataLength, length);
    if (read_size > 0){
        memcpy(pData, pBlock->lpbyBuffer + pBlock->iDataOffset, read_size);
    }
    return read_size;
}

////////////////////////////////////////////////////////////////////////////////

CMemoryPool::CMemoryPool(int buff_size /*= 4096*/, int buff_num /*= 10*/) :kBufferSize(buff_size)
{
    for (int i = 0; i < buff_num; ++i){
        LPMEMORY_BLOCK block = NULL;
        if (MemoryBlock_Construct(buff_size, &block))
            m_MemoryBlock_list.push_back(block);
    }
    m_Item_Count = 0;
}

CMemoryPool::~CMemoryPool()
{
    if (0 != m_Item_Count){
        printf("�ڴ�й¶\n");
    }

    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    std::for_each(std::begin(m_MemoryBlock_list), std::end(m_MemoryBlock_list),
        [](LPMEMORY_BLOCK block){ MemoryBlock_Destruct(block); });
    m_MemoryBlock_list.clear();
}

CMemoryItem* CMemoryPool::Construct()
{
    CMemoryItem *pItem = nullptr;
    try{
        pItem = new CMemoryItem(this);
        m_Item_Count++;
    }
    catch (std::bad_alloc){
        return nullptr;
    }
    return pItem;
}

void CMemoryPool::Destruct(CMemoryItem* pItem)
{
    std::lock_guard<std::recursive_mutex> lck(pItem->m_MemoryBlock_list_mutex);
    std::for_each(std::begin(pItem->m_MemoryBlock_list), std::end(pItem->m_MemoryBlock_list),
        [this](LPMEMORY_BLOCK block){ FreeMemoryBlock(block); });
    pItem->m_MemoryBlock_list.clear();
    m_Item_Count--;
}

bool CMemoryPool::GetMemoryBlock(LPMEMORY_BLOCK *block){
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    if (m_MemoryBlock_list.empty())
    {
        if (!MemoryBlock_Construct(kBufferSize, block))
            return false;
    }
    else
    {
        *block = m_MemoryBlock_list.front();
        m_MemoryBlock_list.pop_front();
    }
    return true;
}

bool CMemoryPool::FreeMemoryBlock(LPMEMORY_BLOCK block){
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    block->iDataLength = 0;
    block->iDataOffset = 0;
    m_MemoryBlock_list.push_back(block);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

CMemoryItem::~CMemoryItem()
{
    kMemoryPool->Destruct(this);
}

int CMemoryItem::Cat(const LPBYTE pBuffer, int iLength)
{
    int cat = 0;
    LPBYTE pData = (LPBYTE)pBuffer;
    int iDataLength = iLength;
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    if (m_MemoryBlock_list.size() > 0){
        LPMEMORY_BLOCK tmp = m_MemoryBlock_list.back();
        if (tmp->iDataLength < tmp->dwBufferSize){
            int write_size = MemoryBlock_Cat(tmp, pData, iDataLength);
            cat += write_size;
            pData += write_size;
            iDataLength -= write_size;
        }
    }

    if (iDataLength > 0){
        int count = (iDataLength + kMemoryPool->kBufferSize - 1) / kMemoryPool->kBufferSize;
        for (int i = 0; i < count; ++i){
            LPMEMORY_BLOCK block;
            if (!kMemoryPool->GetMemoryBlock(&block))
                break;

            int write_size = MemoryBlock_Cat(block, pData, iDataLength);
            cat += write_size;
            pData += write_size;
            iDataLength -= write_size;
            m_MemoryBlock_list.push_back(block);
        }
    }

    return cat;
}

int  CMemoryItem::Cat(CMemoryItem* other)
{
    int cat = other->Size();
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    std::lock_guard<std::recursive_mutex> other_lck(other->m_MemoryBlock_list_mutex);
    if (other->m_MemoryBlock_list.size() == 1 && m_MemoryBlock_list.size() > 0){
        LPMEMORY_BLOCK tmp = m_MemoryBlock_list.back();
        LPMEMORY_BLOCK other_tmp = other->m_MemoryBlock_list.front();
        if (tmp->iDataLength + other_tmp->iDataLength <= tmp->dwBufferSize){
            MemoryBlock_Cat(tmp, other_tmp->lpbyBuffer + other_tmp->iDataOffset, other_tmp->iDataLength);
            other->Reset();
        }
    }

    for (auto memory_block = std::begin(other->m_MemoryBlock_list); memory_block != std::end(other->m_MemoryBlock_list);)
    {
        m_MemoryBlock_list.push_back(*memory_block);
        memory_block = other->m_MemoryBlock_list.erase(memory_block);
    }

    return cat;
}

int CMemoryItem::Fetch(BYTE* pData, int length)
{
    LPMEMORY_BLOCK tmp = nullptr;
    int iDataLength = 0;
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    for (auto black = std::begin(m_MemoryBlock_list); black != std::end(m_MemoryBlock_list);)
    {
        tmp = *black;
        if (length <= 0)
            break;
        int read_size = MemoryBlock_Fetch(tmp, pData, length);
        pData += read_size;
        length -= read_size;
        iDataLength += read_size;

        if (tmp->iDataLength == 0){
            black = m_MemoryBlock_list.erase(black);
            kMemoryPool->FreeMemoryBlock(tmp);
        }
        else
            break;
    }
    return iDataLength;
}

int CMemoryItem::Peek(BYTE* pData, int length)
{
    int iDataLength = 0;
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    for (auto black = std::begin(m_MemoryBlock_list); black != std::end(m_MemoryBlock_list); ++black)
    {
        LPMEMORY_BLOCK tmp = *black;
        if (length <= 0)
            break;
        int read_size = MemoryBlock_Peek(tmp, pData, length);
        pData += read_size;
        length -= read_size;
        iDataLength += read_size;
    }
    return iDataLength;
}

int CMemoryItem::Reduce(int length)
{
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    if (length <= 0)
        return Size();
    int last = Size() - length;
    Reset(0, last);
    return last;
}

void CMemoryItem::Reset(int begin /*= 0*/, int end /*= 0*/)
{
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    LPMEMORY_BLOCK tmp = nullptr;
    int pos = 0;
    for (auto black = std::begin(m_MemoryBlock_list); black != std::end(m_MemoryBlock_list); ++black)
    {
        tmp = *black;
        int iDataLength = tmp->iDataLength;
        int black_begin = pos;
        int black_end = pos + iDataLength;
        if (black_end <= begin){
            tmp->iDataLength = 0;
            printf("CMemoryItem::Reset Remove0 [%d,%d)\n", black_begin, black_end);
        }
        else if (black_begin < begin && black_end > begin){
            int reduce = begin - pos;
            tmp->iDataOffset += reduce;
            tmp->iDataLength -= reduce;
            printf("CMemoryItem::Reset Remove1 [%d,%d)\n", black_begin, black_begin + tmp->iDataLength);
        }
        else if (black_begin >= end){
            tmp->iDataLength = 0;
            printf("CMemoryItem::Reset Remove2 [%d,%d)\n", black_begin, black_end);
        }
        else if (black_begin < end && black_end > end){
            tmp->iDataLength = end - pos;
            printf("CMemoryItem::Reset Remove3 [%d,%d)\n", end, black_end);
        }
        pos += iDataLength;
    }

    for (auto black = std::begin(m_MemoryBlock_list); black != std::end(m_MemoryBlock_list);)
    {
        tmp = *black;
        if (tmp->iDataLength == 0){
            black = m_MemoryBlock_list.erase(black);
            kMemoryPool->FreeMemoryBlock(tmp);
        }
        else
            ++black;
    }
    printf("CMemoryItem::Reset [%d,%d)\n", begin, end);
}

int CMemoryItem::Size(){
    std::lock_guard<std::recursive_mutex> lck(m_MemoryBlock_list_mutex);
    return std::accumulate(std::begin(m_MemoryBlock_list),
        std::end(m_MemoryBlock_list),
        0,
        [](int init, LPMEMORY_BLOCK block){return init + block->iDataLength; });
}
