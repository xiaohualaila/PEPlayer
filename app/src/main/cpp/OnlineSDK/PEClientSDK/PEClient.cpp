#include "PEClient.h"
#include "../MainController.h"

extern void PEThrowException(DWORD dwType, LONG lHandle);
extern void PEPushStreamData(CONST CPEClient* client, LPPE_STREAM_DATA lpInfo, BYTE* pData, int iLength);
extern void PEPushFileData(CONST CPEClient *client, LPPE_FILE_DATA lpInfo, BYTE* pData, int iLength);

/////////////////////////////////////////////
void Common_StreamDataCallBack(LPPE_STREAM_DATA lpInfo, BYTE* pData, int iLength, void *UserData)
{
    CPEClient *client = (CPEClient*)UserData;
    
    uchar *pData_RAW = pData;
    int length_RAW = iLength;

    PEPushStreamData(client, lpInfo, pData, iLength);
}

void Common_FileDataCallBack(LPPE_FILE_DATA lpInfo, BYTE* pData, int iLength, void *UserData)
{
    CPEClient *client = (CPEClient*)UserData;

    uchar *pData_RAW = pData;
    int length_RAW = iLength;

    PEPushFileData(client, lpInfo, pData, iLength);
}

CPEClient::CPEClient()
{
    CClient::SetStreamDataCallBack(Common_StreamDataCallBack,this);
    CClient::SetFileDataCallBack(Common_FileDataCallBack, this);
    ClientAlloc();
}

CPEClient::~CPEClient()
{
    Logout();
    ClientRelease();
}
#define LOGIN_SUCCESS 0
#define LOGIN_FAILED_TIMEOUT 1
#define LOGIN_FAILED_USERERROR 2
int CPEClient::Login(const char *ipaddr, WORD port, const char* username, const char* password)
{
    if (!Connect(ipaddr, port))
        return PE_ERRCODE_NETWORK;

    int loginStatus = CClient::Login(username, password);
    if (loginStatus != LOGIN_SUCCESS) {
        if (loginStatus == LOGIN_FAILED_USERERROR) {
            return PE_ERRCODE_USER_PWD;
        } else if (loginStatus == LOGIN_FAILED_TIMEOUT) {
            return PE_ERRCODE_TIMEOUT;
        }
    }
    return loginStatus;

    return PE_ERRCODE_SUCCESS;
}

int CPEClient::Logout()
{
    CClient::Logout();

    Disconnect();
    
    return PE_ERRCODE_SUCCESS;
}

bool CPEClient::StopRealPlay(int nHandle)
{
    return StopStreamData(nHandle);

}

int CPEClient::GetPEStreamPort(const char* uuid)
{
    return CClient::GetStreamPort(uuid);
}

int CPEClient::GetSadpInfoList(LPPE_CLIENT_SADPINFO_LIST lpSadpInfoList)
{
    RemotePanoSetup *lps;
    size_t ps_num=0;
    if (CClient::GetCamList(&lps,&ps_num))
    {
        lpSadpInfoList->wSadpNum = ps_num;
        LOGI("NUM:%d",ps_num);
        for (size_t i = 0; i < ps_num; ++i)
        {
            LPPE_CLIENT_SADPINFO lpSadpInfo = lpSadpInfoList->struSadpInfo + i;
            LPRemotePanoSetup ps = lps + i;
            strcpy(lpSadpInfo->sIP, ps->ip);
            strcpy(lpSadpInfo->sUUID, ps->uuid);
            strcpy(lpSadpInfo->sDevName, ps->devName);
            strcpy(lpSadpInfo->sSerialNo, ps->serial_no);
        }
        CClient::FreeCamList(lps);
        return PE_ERRCODE_SUCCESS;
    }
    return PE_ERRCODE_FAIL;
}
