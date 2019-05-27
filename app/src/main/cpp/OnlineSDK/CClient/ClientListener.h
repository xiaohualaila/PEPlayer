#ifndef CAM360_LIBCAM_CLIENT_CLIENTLISTENER_H_
#define CAM360_LIBCAM_CLIENT_CLIENTLISTENER_H_
#include "../PanoDef.h"
#include "../type.h"
#include <string>
struct WSABUF {
    ULONG len;     /* the length of the buffer */
    char * buf; /* the pointer to the buffer */
};

class CClientListener
{
protected:
    virtual bool OnClientReceive(PanoComm pc, BYTE *msg_body, size_t msg_body_size) = 0;
    virtual bool OnClientClose() = 0;

    virtual bool Connect(const char *ipaddr, USHORT port) = 0;
    virtual bool Disconnect() = 0;
    virtual std::string GetHost() = 0;
    virtual void ClientAlloc() = 0;
    virtual void ClientRelease() = 0;

    virtual bool Send(const BYTE* pBuffer, int iLength, int iOffset) = 0;
    virtual bool SendPackets(const WSABUF pBuffers[], int iCount) = 0;
};

#endif
