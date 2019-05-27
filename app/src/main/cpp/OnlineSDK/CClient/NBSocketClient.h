#ifndef CAM360_LIBCAM_CLIENT_NBSOCKETCLIENT_H_
#define CAM360_LIBCAM_CLIENT_NBSOCKETCLIENT_H_
#include "ClientListener.h"
#include "MemoryPool.h"
#include <atomic>
#include <thread>
#include <deque>
#include <mutex>

class CNBSocketClient : public CClientListener
{
public:
    CNBSocketClient();
    ~CNBSocketClient();

protected:
    virtual bool Connect(const char *ipaddr, USHORT port) override;
    virtual bool Disconnect() override;
    virtual std::string GetHost() override;
    virtual void ClientAlloc() override;
    virtual void ClientRelease() override;

    virtual bool Send(const BYTE* pBuffer, int iLength, int iOffset) override;
    virtual bool SendPackets(const WSABUF pBuffers[], int iCount) override;

private:
    int m_socket;
    bool m_isConnected;
    std::thread m_WorkerThread;
    std::mutex        m_send_deque_mutex;
    CMemoryItem      *m_send_buff;
    CMemoryItem      *m_worker_send_buff;
    CMemoryItem      *m_worker_recv_buff;
    CMemoryPool       m_bufferpool;

    void WorkerThread();
    bool nbRead(bool *eof);
    bool nbWrite();

    bool nonFatalError();
    int getError();

    bool OnReceive();
    void OnError();
};

#endif
