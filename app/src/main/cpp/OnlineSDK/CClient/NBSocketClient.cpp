#if defined(_WINDOWS)
# include <stdio.h>
//#define FD_SETSIZE      1024
# include <winsock2.h>
# pragma comment( lib, "ws2_32.lib" )
#else
extern "C" {
# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <errno.h>
# include <fcntl.h>
}
#endif  // _WINDOWS

#include "NBSocketClient.h"
#include <bitset>
#include <iostream>
#include <vector>

CNBSocketClient::CNBSocketClient()
{
    m_socket = -1;
    m_isConnected = false;
    m_send_buff = m_bufferpool.Construct();
    m_worker_send_buff = m_bufferpool.Construct();
    m_worker_recv_buff = m_bufferpool.Construct();
}

CNBSocketClient::~CNBSocketClient()
{
    delete m_send_buff;
    delete m_worker_send_buff;
    delete m_worker_recv_buff;
}

bool CNBSocketClient::Connect(const char *host, USHORT port)
{
    if (m_socket == -1)
        return false;

    if (m_isConnected)
        return true;

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;

    struct hostent *hp = gethostbyname(host);
    if (hp == 0) return false;

    saddr.sin_family = hp->h_addrtype;
    memcpy(&saddr.sin_addr, hp->h_addr, hp->h_length);
    saddr.sin_port = htons((u_short)port);

    // For asynch operation, this will return EWOULDBLOCK (windows) or
    // EINPROGRESS (linux) and we just need to wait for the socket to be writable...
    int result = ::connect(m_socket, (struct sockaddr *)&saddr, sizeof(saddr));
    m_isConnected = result == 0 || nonFatalError();
    
    if (m_isConnected){
        m_WorkerThread = std::move(std::thread(std::bind(&CNBSocketClient::WorkerThread, this)));
    }

    return m_isConnected;
}

bool CNBSocketClient::Disconnect()
{
    if (m_isConnected){
        m_isConnected = false;
        if (m_WorkerThread.joinable())
            m_WorkerThread.join();
    }
    ClientRelease();
    ClientAlloc();
    return true;
}

std::string CNBSocketClient::GetHost(){
//    std::string HostStream;
//    struct sockaddr_in saddr;
//    memset(&saddr, 0, sizeof(saddr));
//    int sizeof_saddr = sizeof(saddr);
//    if (0 == ::getsockname(m_socket, (struct sockaddr *)&saddr, &sizeof_saddr))
//    {
//        char chIP[100];
//        char * tmp = inet_ntoa(saddr.sin_addr);
//        memcpy(chIP, tmp, 100);
//        HostStream = chIP;
//    }
//
//    return HostStream;
    return "127.0.0.1:8080";
}

#if defined(_WINDOWS)
static void initWinSock()
{
    static bool wsInit = false;
    if (!wsInit)
    {
        WORD wVersionRequested = MAKEWORD(2, 0);
        WSADATA wsaData;
        WSAStartup(wVersionRequested, &wsaData);
        wsInit = true;
    }
}
#else
#define initWinSock()
#endif // _WINDOWS

void CNBSocketClient::ClientAlloc()
{
    initWinSock();
    m_socket = (int) ::socket(AF_INET, SOCK_STREAM, 0);

    if (m_socket == -1)
        return;

#if defined(_WINDOWS)
    unsigned long flag = 1;
    bool ret = ioctlsocket((SOCKET)m_socket, FIONBIO, &flag) == 0;
#else
    bool ret = fcntl(m_socket, F_SETFL, O_NONBLOCK) == 0;
#endif // _WINDOWS
}

void CNBSocketClient::ClientRelease()
{
    if (m_socket == -1)
        return;

#if defined(_WINDOWS)
    closesocket(m_socket);
#else
    ::close(m_socket);
#endif // _WINDOWS
    m_socket = -1;
}

bool CNBSocketClient::Send(const BYTE* pBuffer, int iLength, int iOffset)
{
    if (!m_isConnected)
        return false;

    std::lock_guard<std::mutex> lck(m_send_deque_mutex);
    return iLength == m_send_buff->Cat((LPBYTE)(pBuffer + iOffset), iLength);
}

bool CNBSocketClient::SendPackets(const WSABUF pBuffers[], int iCount)
{
    if (!m_isConnected)
        return false;

    bool ret = true;
    std::lock_guard<std::mutex> lck(m_send_deque_mutex);
    for (int i = 0; i < iCount; ++i){
        ret &= pBuffers[i].len == m_send_buff->Cat((LPBYTE)(pBuffers[i].buf), pBuffers[i].len);
    }
    return ret;
}

void CNBSocketClient::WorkerThread()
{
    enum EnNBSocketError
    {
        NBSE_UNKNOWN,	// Unknown
        NBSE_CONNECT,	// Connect
        NBSE_SEND,	// Send
        NBSE_RECEIVE,	// Receive
        numEnNBSocketError
    };
    std::bitset<numEnNBSocketError> NBSocketError;

    m_worker_send_buff->Reset();
    m_worker_recv_buff->Reset();

    while (m_isConnected){
        std::unique_lock<std::mutex> lck(m_send_deque_mutex);
        m_worker_send_buff->Cat(m_send_buff);
        lck.unlock();

        bool eof=false;
        if (!nbRead(&eof))
            NBSocketError.set(NBSE_RECEIVE);

        if (eof)
            NBSocketError.set(NBSE_CONNECT);

        if (!nbWrite())
            NBSocketError.set(NBSE_SEND);
        
        if (NBSocketError.any()){
            OnError();
            break;
        }
        else{
            if (!OnReceive())
                break;
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(20));
        std::this_thread::yield();
    }

    while (m_isConnected){
        std::unique_lock<std::mutex> lck(m_send_deque_mutex);
        m_worker_send_buff->Reset();
        lck.unlock();
    }
}

// Read available text from the specified socket. Returns false on error.
bool CNBSocketClient::nbRead(bool *eof)
{
    const int READ_SIZE = 4096;   // Number of bytes to attempt to read at a time
    char readBuf[READ_SIZE];

    bool wouldBlock = false;
    *eof = false;

    while (!wouldBlock && !*eof) {
#if defined(_WINDOWS)
        int n = recv(m_socket, readBuf, READ_SIZE, 0);
#else
        int n = read(m_socket, readBuf, READ_SIZE);
#endif

        if (n > 0) {
            m_worker_recv_buff->Cat((LPBYTE)(readBuf), n);
        }
        else if (n == 0) {
            *eof = true;
        }
        else if (nonFatalError()) {
            wouldBlock = true;
        }
        else {
            return false;   // Error
        }
    }
    return true;
}


// Write text to the specified socket. Returns false on error.
bool CNBSocketClient::nbWrite()
{
    int nToWrite = m_worker_send_buff->Size();
    const int WRITE_SIZE = 4096;   // Number of bytes to attempt to read at a time
    char writeBuf[WRITE_SIZE];
    bool wouldBlock = false;

    while (nToWrite > 0 && !wouldBlock) {
        int n = m_worker_send_buff->Peek((LPBYTE)(writeBuf), WRITE_SIZE);
#if defined(_WINDOWS)
        n = send(m_socket, writeBuf, n, 0);
#else
        n = write(m_socket, writeBuf, n);
#endif

        if (n > 0) {
            m_worker_send_buff->Reset(n,nToWrite);
            nToWrite -= n;
        }
        else if (nonFatalError()) {
            wouldBlock = true;
        }
        else {
            return false;   // Error
        }
    }
    return true;
}

// These errors are not considered fatal for an IO operation; the operation will be re-tried.
bool CNBSocketClient::nonFatalError()
{
    int err = CNBSocketClient::getError();
    return (err == EINPROGRESS || err == EWOULDBLOCK);//|| err == EAGAIN || err == EINTR
}

// Returns last errno
int CNBSocketClient::getError()
{
#if defined(_WINDOWS)
    int ret = WSAGetLastError();

    switch (ret)
    {
    case WSAEINPROGRESS:
        return EINPROGRESS;
        break;
    case WSAEWOULDBLOCK:
        return EWOULDBLOCK;
        break;
    case WSAETIMEDOUT:
        return ETIMEDOUT;
        break;
    default:
        return ret;
        break;
    }
#else
    return errno;
#endif
}

bool CNBSocketClient::OnReceive()
{
    try
    {
        int iLength = m_worker_recv_buff->Size();
        const size_t pc_size = PanoComm_Size();
        const size_t data_size = sizeof(PE_StreamZeroChanData);

        PanoComm pc;
        memset(&pc, 0, sizeof(pc));
        while (pc_size == m_worker_recv_buff->Peek((BYTE *)&pc, pc_size))
        {
            if (PanoComm_Check(&pc))
            {
                if (pc.pkgSize > iLength)
                    return true;

                int   msg_type = pc.command;
                BYTE *msg_buff = new BYTE[pc.pkgSize];
                size_t msg_buff_size = pc.pkgSize;
                BYTE *msg_body = msg_buff + pc_size;
                size_t msg_body_size = msg_buff_size - pc_size;

                int ret = m_worker_recv_buff->Fetch(msg_buff, msg_buff_size);
                if (ret == msg_buff_size)
                {
                    OnClientReceive(pc, msg_body, msg_body_size);
                    iLength -= msg_buff_size;
                }
                else{
                    //TRACE2("CClient::OnReceive(), FR_LENGTH_TOO_LONG, file: %s, line: %d\n", __FILE__, __LINE__);
                    delete[] msg_buff;
                    return true;
                }
                delete[] msg_buff;
                memset(&pc, 0, sizeof(pc));
            }
            else
            {
                BYTE pData;
                m_worker_recv_buff->Fetch(&pData, 1);
            }
        }
    }
    catch (std::bad_alloc)
    {
    }
    return true;

}

void CNBSocketClient::OnError()
{
    OnClientClose();
}
