#ifndef CAM360_PANOCAM_NETCLIENT_HPSOCKET_H_
#define CAM360_PANOCAM_NETCLIENT_HPSOCKET_H_
#include "../CClient/CClient.h"
#include "PEClientSDK.h"
#include <memory>
#include <exception>
#include <thread>
#include <mutex>
#include <functional>

class CPEClient : public CClient
{
public:
    CPEClient();
    virtual ~CPEClient();

    int Login(const char *ipaddr, WORD port, const char* username, const char* password);
    int Logout();
    int GetSadpInfoList(LPPE_CLIENT_SADPINFO_LIST lpSadpInfoList);

    bool StopRealPlay(int);
    int GetPEStreamPort(const char* uuid);
};

#endif //CAM360_PANOCAM_NETCLIENT_HPSOCKET_H_