#if (defined(WIN32) || defined(_WIN32_WCE))
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#elif defined(__linux__) || defined(__APPLE__) //linux
//#include "../CrossPlatformCDefine/CrossPlatformCDefine.h"
#endif
#include "PEClientSDK.h"
#include "PEClient.h"
//#include "../../Common.h"
#include <android/log.h>
#define LOG_TAG    "JNI_LOG"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#include <algorithm>
#include <atomic>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <string>

#define PE_MAX_CLIENT     200
#define PE_MAX_PREVIEW    2000
#define PE_MAX_FIND       2000
#define PE_MAX_PLAYBACK   2000
#define PE_MAX_DOWNLOAD   2000

typedef struct tag_PE_CLIENT            PE_CLIENT;
typedef struct tag_PE_CLIENT           *LPPE_CLIENT;
typedef struct tag_PE_CLIENT_PREVIEW    PE_CLIENT_PREVIEW;
typedef struct tag_PE_CLIENT_PREVIEW   *LPPE_CLIENT_PREVIEW;
typedef struct tag_PE_CLIENT_FIND       PE_CLIENT_FIND;
typedef struct tag_PE_CLIENT_FIND      *LPPE_CLIENT_FIND;
typedef struct tag_PE_CLIENT_PLAYBACK   PE_CLIENT_PLAYBACK;
typedef struct tag_PE_CLIENT_PLAYBACK  *LPPE_CLIENT_PLAYBACK;
typedef struct tag_PE_CLIENT_DOWNLOAD   PE_CLIENT_DOWNLOAD;
typedef struct tag_PE_CLIENT_DOWNLOAD  *LPPE_CLIENT_DOWNLOAD;

struct tag_PE_CLIENT{
    bool unused;
    char sDVRIP[200];
    WORD wDVRPort;
    char sUserName[200];
    char sPassword[200];
    LONG lUserID;
    CPEClient *lpclient;
};

struct tag_PE_CLIENT_PREVIEW{
    bool unused;
    PE_CLIENT_PREVIEWINFO preview_info;
    PE_CLIENT_SADPINFO sadp_info;
    LONG lUserID;
    LONG lRealHandle;
    INT  nHandle;
    fPERealDataCallBack cbRealDataCallBack;
    void* pUserData;
    DWORD lSequenceId[256];
};

struct tag_PE_CLIENT_FIND{
    bool unused;
    LONG lUserID;
    LONG lFindHandle;
    long lIndex;
    long lDataNum;
    LPPE_CLIENT_FINDDATA lpData;
};

struct tag_PE_CLIENT_PLAYBACK{
    bool unused;
    CHAR sFileName[250];
    PE_CLIENT_PREVIEWINFO preview_info;
    PE_CLIENT_SADPINFO sadp_info;
    PE_CLIENT_FINDDATA find_data;
    LONG lUserID;
    LONG lPlayHandle;
    INT  nHandle;
    fPEPlayDataCallBack cbPlayDataCallBack;
    void* pUserData;
    int play_speed;
    FLOAT total_time;
    struct tm start_time;
    DWORD lSequenceId[256];
};

struct tag_PE_CLIENT_DOWNLOAD{
    bool unused;
    LONG lUserID;
    LONG lFileHandle;
    INT  nHandle;
    FILE *fp;
    DWORD dwFileSize;
    DWORD dwRecvLength;
    std::string sFileName;
};

static volatile long long                   g_sdk_inited;
static volatile long                        g_api_last_error_code;
static std::recursive_mutex                 g_api_recursive_mutex; // recursive_mutex 是一种递归互斥锁类

static fPEExceptionCallBack                 g_exception_callback = NULL;
static void*                                g_exception_userdata = NULL;

PE_CLIENT                                   g_pe_client[PE_MAX_CLIENT];
PE_CLIENT_PREVIEW                           g_pe_preview[PE_MAX_PREVIEW];
PE_CLIENT_FIND                              g_pe_find[PE_MAX_FIND];
PE_CLIENT_PLAYBACK                          g_pe_playback[PE_MAX_PLAYBACK];
PE_CLIENT_DOWNLOAD                          g_pe_download[PE_MAX_DOWNLOAD];

inline bool SelectLPPE_CLIENTByUserID(LONG lUserID, LPPE_CLIENT *client){
    if (lUserID < 0 || lUserID >= PE_MAX_CLIENT)
    {
        g_api_last_error_code = PE_ERRCODE_OUTOFMEM;
        return false;
    }
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    if (g_pe_client[lUserID].unused || NULL == g_pe_client[lUserID].lpclient)
    {
        g_api_last_error_code = PE_ERRCODE_NULLPOINT;
        return false;
    }
    else{
        *client = g_pe_client + lUserID;
        return true;
    }
}

inline bool SelectLPPE_CLIENT_PREVIEWByRealHandle(LONG lRealHandle, LPPE_CLIENT_PREVIEW *preview){
    if (lRealHandle < 0 || lRealHandle >= PE_MAX_PREVIEW)
    {
        g_api_last_error_code = PE_ERRCODE_NOT_FIND;
        return false;
    }
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    if (g_pe_preview[lRealHandle].unused)
    {
        g_api_last_error_code = PE_ERRCODE_NULLPOINT;
        return false;
    }
    else{
        *preview = g_pe_preview + lRealHandle;
        return true;    
    }
}

inline bool SelectLPPE_CLIENT_FILE_LISTByFindHandle(LONG lFindHandle, LPPE_CLIENT_FIND *lplppe_find)
{
    if (lFindHandle < 0 || lFindHandle >= PE_MAX_FIND)
    {
        g_api_last_error_code = PE_ERRCODE_NOT_FIND;
        return false;
    }
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    if (g_pe_find[lFindHandle].unused){
        g_api_last_error_code = PE_ERRCODE_NULLPOINT;
        return false;
    }
    *lplppe_find = g_pe_find + lFindHandle;
    return true;
}

inline bool SelectLPPE_CLIENT_PLAYBACKByPlayHandle(LONG lPlayHandle, LPPE_CLIENT_PLAYBACK *playback){
    if (lPlayHandle < 0 || lPlayHandle >= PE_MAX_PLAYBACK)
    {
        g_api_last_error_code = PE_ERRCODE_OUTOFMEM;
        return false;
    }
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_PLAYBACK lppe_playback = g_pe_playback + lPlayHandle;
    if (g_pe_playback[lPlayHandle].unused)
    {
        g_api_last_error_code = PE_ERRCODE_NULLPOINT;
        return false;
    }
    else{
        *playback = g_pe_playback + lPlayHandle;
        return true;
    }
}

inline bool SelectLPPE_CLIENT_DOWNLOADByFileHandle(LONG lFileHandle, LPPE_CLIENT_DOWNLOAD *download){
    if (lFileHandle < 0 || lFileHandle >= PE_MAX_DOWNLOAD)
    {
        g_api_last_error_code = PE_ERRCODE_NOT_FIND;
        return false;
    }
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    if (g_pe_download[lFileHandle].unused)
    {
        g_api_last_error_code = PE_ERRCODE_NULLPOINT;
        return false;
    }
    else{
        *download = g_pe_download + lFileHandle;
        return true;
    }
}

inline bool SelectLPPE_CLIENTByRealHandle(LONG lRealHandle, LPPE_CLIENT *client){
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_PREVIEW lppe_preview = NULL;
    if (!SelectLPPE_CLIENT_PREVIEWByRealHandle(lRealHandle, &lppe_preview))
        return false;

    return SelectLPPE_CLIENTByUserID(lppe_preview->lUserID, client);
}

inline bool SelectLPPE_CLIENT_SADPINFOByUserIDAndChannel(LONG lUserID, LONG lChannel, LPPE_CLIENT_SADPINFO sadp_info)
{
    PE_CLIENT_SADPINFO_LIST sadp_info_list;
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    if (TRUE == PE_Client_GetSadpInfoList(lUserID, &sadp_info_list)){
        if (lChannel < 0 || lChannel >= sadp_info_list.wSadpNum)
        {
            g_api_last_error_code = PE_ERRCODE_PARAM;
            return false;
        }

        memcpy(sadp_info, sadp_info_list.struSadpInfo + lChannel, sizeof(PE_CLIENT_SADPINFO));
        return true;
    }
    return false;
}

void PEThrowException(DWORD dwType, LONG lHandle)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    if (g_exception_callback)
        g_exception_callback(dwType, lHandle, g_exception_userdata);
}

inline void Struct_PE_AVFRAME_DATA_Init(CONST LPPE_STREAM_DATA lpInfo, CONST BYTE* pData, CONST int iLength, LPPE_AVFRAME_DATA data)
{
    data->byStreamFormat = 1;                                  //1表示原始流，2表示混合流
    data->byESStreamType = 1;                                  //原始流类型，1表示视频，2表示音频
    data->byEncoderType = PE_ENCODER_TYPE_H264;                //编码格式,其值请参看PE_ENCODER_TYPE枚举定义
    data->byFrameType = lpInfo->byFrameType;                   //数据帧类型,1表示I帧, 2表示P帧, 0表示未知类型
    data->wFrameRate = 0;                                      //帧率
    data->wBitRate = 0;                                        //当前码率
    data->byChannel = lpInfo->byPID;                           //通道
    //data->lSequenceId;                                         //数据帧序号
    data->pszData = (CHAR*)pData;                              //数据
    data->lDataLength = iLength;                               //数据有效长度
    data->lImageWidth = lpInfo->lImageWidth;                   //视频宽度
    data->lImageHeight = lpInfo->lImageHeight;                 //视频高度
    data->lTimeStamp = lpInfo->llTimeStamp * 1000;             //数据采集时间戳，单位为微秒
}

inline void Struct_PE_AVFRAME_DATA_InitMux(CONST LPPE_STREAM_DATA lpInfo, CONST BYTE* pData, CONST int iLength, LPPE_AVFRAME_DATA data)
{
    const size_t head_size = sizeof(PE_AVFRAME_DATA);
    size_t buff_size = head_size + iLength;
    char *buff = new char[buff_size];
    Struct_PE_AVFRAME_DATA_Init(lpInfo, pData, iLength, LPPE_AVFRAME_DATA(buff));
    memcpy(buff + head_size, pData, iLength);

    memcpy(data, buff, head_size);
    data->byStreamFormat = 2;
    data->byChannel = 0;
    data->lSequenceId = 0;
    data->pszData = buff;
    data->lDataLength = buff_size;
}

bool PEPushRealStreamData(LONG lUserID, LPPE_STREAM_DATA lpInfo, BYTE* pData, int iLength){
    LONG lRealHandle = -1;
    LPPE_CLIENT_PREVIEW lppe_preview = NULL;
    for (LONG handle = 0; handle < PE_MAX_PREVIEW; ++handle){
        lppe_preview = g_pe_preview + handle;
        if (!lppe_preview->unused
            && lppe_preview->lUserID == lUserID
            && lppe_preview->nHandle == lpInfo->byPreviewID){
            lRealHandle = lppe_preview->lRealHandle;
            break;
        }
    }
    if (-1 == lRealHandle)
        return false;

    fPERealDataCallBack cbRealDataCallBack = lppe_preview->cbRealDataCallBack;
    if (NULL == cbRealDataCallBack)
        return true;

    PE_AVFRAME_DATA data;
    //Struct_PE_AVFRAME_DATA_Init(lpInfo, pData, iLength, &data);
    //data.lSequenceId = lppe_preview->lSequenceId++;
    Struct_PE_AVFRAME_DATA_InitMux(lpInfo, pData, iLength, &data);
    LPPE_AVFRAME_DATA(data.pszData)->lSequenceId = lppe_preview->lSequenceId[lpInfo->byPID]++;

    cbRealDataCallBack(lRealHandle,
        lppe_preview->sadp_info.sSerialNo, lppe_preview->preview_info.dwStreamType,
        &data, lppe_preview->pUserData);

    delete[] data.pszData;
    return true;
}

bool PEPushPlaybackStreamData(LONG lUserID, LPPE_STREAM_DATA lpInfo, BYTE* pData, int iLength){
    LONG lPlayHandle = -1;
    LPPE_CLIENT_PLAYBACK lppe_playback = NULL;
    for (LONG handle = 0; handle < PE_MAX_PLAYBACK; ++handle){
        lppe_playback = g_pe_playback + handle;
        if (!lppe_playback->unused
            && lppe_playback->lUserID == lUserID
            && lppe_playback->nHandle == lpInfo->byPreviewID){
            lPlayHandle = lppe_playback->lPlayHandle;
            break;
        }
    }
    if (-1 == lPlayHandle)
        return false;

    fPEPlayDataCallBack cbPlayDataCallBack = lppe_playback->cbPlayDataCallBack;
    if (NULL == cbPlayDataCallBack)
        return true;

    PE_AVFRAME_DATA data;
    //Struct_PE_AVFRAME_DATA_Init(lpInfo, pData, iLength, &data);
    //data.lSequenceId = lppe_preview->lSequenceId++;
    Struct_PE_AVFRAME_DATA_InitMux(lpInfo, pData, iLength, &data);
    LPPE_AVFRAME_DATA(data.pszData)->lSequenceId = lppe_playback->lSequenceId[lpInfo->byPID]++;

    cbPlayDataCallBack(lPlayHandle,
        lppe_playback->sFileName, lppe_playback->preview_info.dwStreamType,
        &data, lppe_playback->pUserData);

    delete[] data.pszData;
    return true;
}

void PEPushStreamData(CONST CPEClient *client, LPPE_STREAM_DATA lpInfo, BYTE* pData, int iLength){
    LONG lUserID = -1;
    for (LONG handle = 0; handle < PE_MAX_CLIENT; ++handle){
        if (!g_pe_client[handle].unused && client == g_pe_client[handle].lpclient){
            lUserID = handle;
            break;
        }
    }
    if (-1 == lUserID)
        return;
    
    if (!PEPushRealStreamData(lUserID, lpInfo, pData, iLength))
        PEPushPlaybackStreamData(lUserID, lpInfo, pData, iLength);
}

void PEPushFileData(CONST CPEClient *client, LPPE_FILE_DATA lpInfo, BYTE* pData, int iLength)
{
    LONG lUserID = -1;
    for (LONG handle = 0; handle < PE_MAX_CLIENT; ++handle){
        if (!g_pe_client[handle].unused && client == g_pe_client[handle].lpclient){
            lUserID = handle;
            break;
        }
    }
    if (-1 == lUserID)
        return;

    LONG lFileHandle = -1;
    LPPE_CLIENT_DOWNLOAD lppe_download = NULL;
    for (LONG handle = 0; handle < PE_MAX_PREVIEW; ++handle){
        lppe_download = g_pe_download + handle;
        if (!lppe_download->unused
            && lppe_download->lUserID == lUserID
            && lppe_download->nHandle == lpInfo->byPreviewID){
            lFileHandle = lppe_download->lFileHandle;
            break;
        }
    }
    if (-1 == lFileHandle)
        return;

    lppe_download->dwRecvLength += iLength;
    //fseek(lppe_download->fp, lpInfo->dwOffset,SEEK_SET);
    fwrite(pData, 1, iLength, lppe_download->fp);
}

BOOL WINAPI PE_Client_Init()
{
    // lock_guard 是一种锁类的类模板 这里使用类模板定义了模板类对象lck，
    // 会调用lock_guard类的默认构造函数，对递归互斥锁对象g_api_recursive_mutex进行上锁。
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    g_sdk_inited++;
    if (1 == g_sdk_inited)
    {
        ////////////////////////////////////////////////
        for (int i = 0; i < PE_MAX_CLIENT; ++i){
            g_pe_client[i].unused = true;
            g_pe_client[i].lUserID = i;
            g_pe_client[i].lpclient = NULL;
        }

        for (int i = 0; i < PE_MAX_PREVIEW; ++i){
            g_pe_preview[i].unused = true;
            g_pe_preview[i].lRealHandle = i;
        }

        for (int i = 0; i < PE_MAX_FIND; ++i){
            g_pe_find[i].unused = true;
            g_pe_find[i].lFindHandle = i;
            g_pe_find[i].lIndex = 0;
            g_pe_find[i].lDataNum = 0;
            g_pe_find[i].lpData = NULL;
        }

        for (int i = 0; i < PE_MAX_PLAYBACK; ++i){
            g_pe_playback[i].unused = true;
            g_pe_playback[i].lPlayHandle = i;
        }

        for (int i = 0; i < PE_MAX_DOWNLOAD; ++i){
            g_pe_download[i].unused = true;
            g_pe_download[i].lFileHandle = i;
        }
    }
    return TRUE;
}

BOOL WINAPI PE_Client_Cleanup()
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    g_sdk_inited--;
    if (0 == g_sdk_inited)
    {
        ////////////////////////////////////////////////
        g_sdk_inited = 1;


        g_sdk_inited = 0;
    }
    else if (g_sdk_inited < 0){
        g_sdk_inited = 0;
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI PE_Client_SetExceptionCallBack(fPEExceptionCallBack cbExceptionCallBack, void* pUserData)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    g_exception_callback = cbExceptionCallBack;
    g_exception_userdata = pUserData;
    return TRUE;
}

LONG WINAPI PE_Client_Login(CONST CHAR *sDVRIP, WORD wDVRPort, CONST CHAR *sUserName, CONST CHAR *sPassword)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT lppe_client = NULL;
    for (LONG handle = 0; handle < PE_MAX_CLIENT; ++handle){
        if (g_pe_client[handle].unused){
            lppe_client = g_pe_client + handle;
            break;
        }
    }
    if (NULL == lppe_client){
        g_api_last_error_code = PE_ERRCODE_OUTOFMEM;
        return -1;
    }

    CPEClient * lpclient = new CPEClient;
    int error_code = lpclient->Login(sDVRIP, wDVRPort, sUserName, sPassword);
    if (error_code != PE_ERRCODE_SUCCESS){
        delete lpclient;
        g_api_last_error_code = error_code;
        return -1;
    }
    else{
        lppe_client->lpclient = lpclient;
        strcpy(lppe_client->sDVRIP, sDVRIP);
        lppe_client->wDVRPort = wDVRPort;
        strcpy(lppe_client->sUserName, sUserName);
        strcpy(lppe_client->sPassword, sPassword);
        lppe_client->unused = false;
        return lppe_client->lUserID;
    }
}

BOOL WINAPI PE_Client_Logout(LONG lUserID)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return FALSE;

    delete lppe_client->lpclient;
    lppe_client->lpclient = NULL;
    lppe_client->unused = true;
    return TRUE;
}

LONG WINAPI PE_Client_RealPlay(LONG lUserID, LPPE_CLIENT_PREVIEWINFO lpPreviewInfo, fPERealDataCallBack cbRealDataCallBack, void* pUserData)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);

    LOGI("OLPano：NUM:%ld",lpPreviewInfo->lChannel);

    LPPE_CLIENT_PREVIEW lppe_preview = NULL;
    for (int i = 0; i < PE_MAX_PREVIEW; ++i){
        if (g_pe_preview[i].unused){
            lppe_preview = g_pe_preview + i;
            break;
        }
    }
    if (NULL == lppe_preview){
        g_api_last_error_code = PE_ERRCODE_OUTOFMEM;
        return -1;
    }

    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return -1;

    PE_CLIENT_SADPINFO sadp_info;
    if (!SelectLPPE_CLIENT_SADPINFOByUserIDAndChannel(lUserID, lpPreviewInfo->lChannel, &sadp_info))
        return -1;

    LOGI("PECLIENT：port:%d",lppe_client->wDVRPort);
    ///////////////////////////////////////////////////////////////////
    int port = lppe_client->lpclient->GetPEStreamPort((char*)sadp_info.sUUID);

    LOGI("PECLIENT:wDVRPort：%d",port);

    lppe_client->lpclient->Logout();
    int error_code = lppe_client->lpclient->Login(lppe_client->sDVRIP, port, lppe_client->sUserName, lppe_client->sPassword);
    if (error_code != PE_ERRCODE_SUCCESS){
        delete lppe_client->lpclient;
        lppe_client->lpclient = NULL;
        lppe_client->unused = true;
        g_api_last_error_code = error_code;
        return -1;
    }
    LOGI("PECLIENT:登陆成功：%d",port);

    lppe_client->wDVRPort = port;
    ////////////////////////////////////////////////////////////////////////////
    if (lpPreviewInfo->byProtoType == 0 && lpPreviewInfo->dwLinkMode == 0)
    {
        lppe_preview->lUserID = lUserID;
        memcpy(&(lppe_preview->preview_info), lpPreviewInfo, sizeof(*lpPreviewInfo));
        memcpy(&(lppe_preview->sadp_info), &sadp_info, sizeof(sadp_info));
        lppe_preview->cbRealDataCallBack = cbRealDataCallBack;
        lppe_preview->pUserData = pUserData;
        for (int j = 0; j < 256; ++j)
            lppe_preview->lSequenceId[j] = 0;

        LOGI("码流：%d",lpPreviewInfo->dwStreamType);

        int handle;
        if (lppe_client->lpclient->RequestStreamData(sadp_info.sUUID, lpPreviewInfo->dwStreamType, &handle)){
            lppe_preview->nHandle = handle;
            lppe_preview->unused = false;
            return lppe_preview->lRealHandle;
        }
        else
        {
            g_api_last_error_code = PE_ERRCODE_FAIL;
        }
    }

    return -1;
}

BOOL WINAPI PE_Client_StopRealPlay(LONG lRealHandle)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_PREVIEW lppe_preview = NULL;
    if (!SelectLPPE_CLIENT_PREVIEWByRealHandle(lRealHandle, &lppe_preview))
        return FALSE;

    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lppe_preview->lUserID, &lppe_client))
        return FALSE;

    int nHandle = lppe_preview->nHandle;
    lppe_preview->unused = true;

    lppe_client->lpclient->StopStreamData(nHandle);

    return TRUE;
}

inline void ConvertStructTmToString(const struct tm* tmp, char* buff, int buff_size)
{
    sprintf(buff, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d.%04u",
            tmp->tm_year + 1900,
            tmp->tm_mon + 1,
            tmp->tm_mday,
            tmp->tm_hour,
            tmp->tm_min,
            tmp->tm_sec, 0);
}

template<size_t N>
inline void ConvertStructTmToString(const struct tm* ct, char(&buff)[N]){
    ConvertStructTmToString(ct, buff, N);
}

inline void ConvertStringToStructTm(const char* str, struct tm* tmp)
{
    time_t t = time(NULL);
    *tmp = *localtime(&t);
    sscanf(str, "%d-%d-%d %d:%d:%d",
           &tmp->tm_year,
           &tmp->tm_mon,
           &tmp->tm_mday,
           &tmp->tm_hour,
           &tmp->tm_min,
           &tmp->tm_sec);
    tmp->tm_year -= 1900;
    tmp->tm_mon -= 1;
}

LONG WINAPI PE_Client_FindFile(LONG lUserID, LPPE_CLIENT_FILECOND pFindCond)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_FIND lppe_find = NULL;
    for (int i = 0; i < PE_MAX_FIND; ++i){
        if (g_pe_find[i].unused){
            lppe_find = g_pe_find + i;
            break;
        }
    }
    if (NULL == lppe_find){
        g_api_last_error_code = PE_ERRCODE_OUTOFMEM;
        return -1;
    }

    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return -1;

    PE_CLIENT_SADPINFO sadp_info;
    if (!SelectLPPE_CLIENT_SADPINFOByUserIDAndChannel(lUserID, pFindCond->lChannel, &sadp_info))
        return -1;

    char begTime[256];
    ConvertStructTmToString(&pFindCond->struStartTime, begTime);
    char endTime[256];
    ConvertStructTmToString(&pFindCond->struStopTime, endTime);

    CClient::RemoteRecordExtInfos info;
    if (!lppe_client->lpclient->GetRecordFileList(sadp_info.sUUID, begTime, endTime, &info)){
        g_api_last_error_code = PE_ERRCODE_FAIL;
        return -1;
    }

    lppe_find->unused = false;
    lppe_find->lUserID = lUserID;
    lppe_find->lIndex = 0;
    long lFileDataNum = info.size();
    lppe_find->lDataNum = lFileDataNum;
    if (lFileDataNum > 0){
        lppe_find->lpData = new PE_CLIENT_FINDDATA[lFileDataNum];
        for (int i = 0; i < lFileDataNum; ++i)
        {
            LPPE_CLIENT_FINDDATA find_data = lppe_find->lpData + i;
            strcpy(find_data->sFileName, info[i].full_dir.c_str());                         //文件名
            ConvertStringToStructTm(info[i].record_time.c_str(), &find_data->struStartTime);//文件的开始时间
            ConvertStringToStructTm(info[i].end_time.c_str(), &find_data->struStopTime);    //文件的结束时间
            find_data->dwFileSize=0;                                                        //文件的大小
            find_data->byLocked=0;                                                          //1表示此文件已经被锁定,0表示正常的文件
        }
    }
    return lppe_find->lFindHandle;
}

BOOL WINAPI PE_Client_FindNextFile(LONG lFindHandle, LPPE_CLIENT_FINDDATA lpFindData)
{
    LPPE_CLIENT_FIND lppe_find = NULL;
    if (!SelectLPPE_CLIENT_FILE_LISTByFindHandle(lFindHandle, &lppe_find))
        return FALSE;

    if (lppe_find->lIndex >= lppe_find->lDataNum){
        g_api_last_error_code = PE_ERRCODE_NOT_FIND;
        return FALSE;
    }

    memcpy(lpFindData, lppe_find->lpData + lppe_find->lIndex, sizeof(PE_CLIENT_FINDDATA));
    lppe_find->lIndex++;

    return TRUE;
}

BOOL WINAPI PE_Client_FindClose(LONG lFindHandle)
{
    LPPE_CLIENT_FIND lppe_find = NULL;
    if (!SelectLPPE_CLIENT_FILE_LISTByFindHandle(lFindHandle, &lppe_find))
        return FALSE;

    if (lppe_find->lDataNum > 0){
        delete[] lppe_find->lpData;
    }

    lppe_find->lIndex = 0;
    lppe_find->lDataNum = 0;
    lppe_find->lpData = NULL;
    lppe_find->unused = true;

    return TRUE;
}

inline bool FindPlayBackInfoByName(LONG lUserID, const char *sPlayBackFileName, LPPE_CLIENT_PREVIEWINFO lppreview_info,
    LPPE_CLIENT_SADPINFO lpsadp_info, LPPE_CLIENT_FINDDATA lpfind_data)
{
    PE_CLIENT_FILECOND find_cond;
    memset(&find_cond, 0, sizeof(find_cond));
    find_cond.struStopTime.tm_year = 9000 - 1900;
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    PE_CLIENT_SADPINFO_LIST sadp_info_list;
    if (TRUE == PE_Client_GetSadpInfoList(lUserID, &sadp_info_list)){
        for (int i = 0; i < sadp_info_list.wSadpNum; ++i){
            find_cond.lChannel = i;
            LONG lFindHandle = PE_Client_FindFile(lUserID, &find_cond);
            if (lFindHandle >= 0){
                PE_CLIENT_FINDDATA find_data;
                while (TRUE == PE_Client_FindNextFile(lFindHandle, &find_data))
                {
                    if (0 == strcmp(find_data.sFileName, sPlayBackFileName)){
                        memset(lppreview_info, 0, sizeof(*lppreview_info));
                        lppreview_info->lChannel = i;
                        lppreview_info->dwStreamType=0;
                        lppreview_info->dwLinkMode=0;
                        lppreview_info->byProtoType=0;

                        memcpy(lpsadp_info, sadp_info_list.struSadpInfo + i, sizeof(*lpsadp_info));

                        memcpy(lpfind_data, &find_data, sizeof(*lpfind_data));
                        
                        PE_Client_FindClose(lFindHandle);
                        return true;
                    }
                }
                PE_Client_FindClose(lFindHandle);
            }
        }
    }
    g_api_last_error_code = PE_ERRCODE_NOT_FIND;
    return false;
}

LONG WINAPI PE_Client_PlayBackByName(LONG lUserID, CONST CHAR *sPlayBackFileName, fPEPlayDataCallBack cbPlayDataCallBack, void* pUserData)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_PLAYBACK lppe_playback = NULL;
    for (int i = 0; i < PE_MAX_PLAYBACK; ++i){
        if (g_pe_playback[i].unused){
            lppe_playback = g_pe_playback + i;
            break;
        }
    }
    if (NULL == lppe_playback){
        g_api_last_error_code = PE_ERRCODE_OUTOFMEM;
        return FALSE;
    }

    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return -1;

    PE_CLIENT_PREVIEWINFO preview_info;
    PE_CLIENT_SADPINFO sadp_info;
    PE_CLIENT_FINDDATA find_data;
    if (!FindPlayBackInfoByName(lUserID, sPlayBackFileName, &preview_info, &sadp_info, &find_data))
        return -1;

    strcpy(lppe_playback->sFileName, sPlayBackFileName);
    memcpy(&lppe_playback->preview_info, &preview_info, sizeof(preview_info));
    memcpy(&lppe_playback->sadp_info, &sadp_info, sizeof(sadp_info));
    memcpy(&lppe_playback->find_data, &find_data, sizeof(find_data));
    lppe_playback->lUserID = lUserID;
    lppe_playback->cbPlayDataCallBack = cbPlayDataCallBack;
    lppe_playback->pUserData = pUserData;
    for (int j = 0; j < 256; ++j)
        lppe_playback->lSequenceId[j] = 0;

    float total_time;
    struct tm recTime;
    int handle;
    if (!lppe_client->lpclient->PlayRecordFile(sadp_info.sUUID, sPlayBackFileName, &total_time, &recTime, &handle)){
        g_api_last_error_code = PE_ERRCODE_FAIL;
        return -1;
    }

    lppe_playback->total_time = total_time;
    memcpy(&lppe_playback->start_time, &recTime, sizeof(recTime));
    lppe_playback->nHandle = handle;
    lppe_playback->unused = false;
    return lppe_playback->lPlayHandle;
}

BOOL WINAPI PE_Client_PlayBackControl(LONG lPlayHandle, DWORD dwControlCode, LPVOID lpInBuffer, DWORD dwInLen, LPVOID lpOutBuffer, DWORD *lpOutLen)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_PLAYBACK lppe_playback = NULL;
    if (!SelectLPPE_CLIENT_PLAYBACKByPlayHandle(lPlayHandle, &lppe_playback))
        return FALSE;
    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lppe_playback->lUserID, &lppe_client))
        return FALSE;

    bool ret = false;
    switch (dwControlCode){
    case PE_PLAYPAUSE: lppe_client->lpclient->PausePlayRecord(lppe_playback->nHandle,true);  break;
    case PE_PLAYRESTART: lppe_client->lpclient->PausePlayRecord(lppe_playback->nHandle, false);  break;
    case PE_PLAYFAST: 
        ++lppe_playback->play_speed;
        lppe_playback->play_speed = fmin(lppe_playback->play_speed, 4);
        ret = lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYSLOW:
        --lppe_playback->play_speed;
        lppe_playback->play_speed = fmax(lppe_playback->play_speed, -4);
        ret = lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYNORMAL:
        lppe_playback->play_speed = 0;
        ret = lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYSETPOS:
    {    
        float cur_tm = *(float*)lpInBuffer * lppe_playback->total_time;
        cur_tm = fmin(cur_tm, lppe_playback->total_time);
        cur_tm = fmax(cur_tm, 0.0f);
        ret = lppe_client->lpclient->SetPlayRecordCurrentTime(lppe_playback->nHandle, cur_tm);
    }
        break;
    case PE_PLAYGETPOS:    
    {
        float cur_tm = 0.0f;
        if (lppe_client->lpclient->GetPlayRecordState(lppe_playback->nHandle, &cur_tm)){
            *(float *)lpOutBuffer = cur_tm / lppe_playback->total_time;
            *lpOutLen = 4;
            ret = true;
        }
    }
        break;
    case PE_PLAYGETTIME: 
    {
        float cur_tm = 0.0f;
        if (lppe_client->lpclient->GetPlayRecordState(lppe_playback->nHandle, &cur_tm)){
            time_t ctt = mktime(&lppe_playback->start_time) + static_cast<time_t>(cur_tm);
            struct tm ct;
            ct = *localtime(&ctt);
            memcpy(lpOutBuffer, &ct, sizeof(ct));
            *lpOutLen = sizeof(ct);
            ret = true;
        }
    }
        break;
    case PE_PLAYSETTIME: 
    {
        struct tm *ct = (struct tm*)lpInBuffer;
        time_t ct1 = mktime(ct);
        time_t ct0 = mktime(&lppe_playback->start_time);
        float cur_tm = ct1 - ct0;
        cur_tm = fmin(cur_tm, lppe_playback->total_time);
        cur_tm = fmax(cur_tm, 0.0f);
        ret = lppe_client->lpclient->SetPlayRecordCurrentTime(lppe_playback->nHandle, cur_tm);
    }
        break;
    }

    if (!ret){
        g_api_last_error_code = PE_ERRCODE_FAIL;
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI PE_Client_StopPlayBack(LONG lPlayHandle)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_PLAYBACK lppe_playback = NULL;
    if (!SelectLPPE_CLIENT_PLAYBACKByPlayHandle(lPlayHandle, &lppe_playback))
        return FALSE;

    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lppe_playback->lUserID, &lppe_client))
        return FALSE;

    int nHandle = lppe_playback->nHandle;
    lppe_playback->unused = true;

    lppe_client->lpclient->StopPlayRecord(nHandle);

    return TRUE;
}

BOOL WINAPI PE_Client_SetServerConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID lpInBuffer, DWORD dwInBufferSize)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return FALSE;

    PE_CLIENT_SADPINFO sadp_info;
    if (!SelectLPPE_CLIENT_SADPINFOByUserIDAndChannel(lUserID, lChannel, &sadp_info))
        return FALSE;
    
    return lppe_client->lpclient->SetServerConfig(sadp_info.sUUID, dwCommand, lpInBuffer, dwInBufferSize) ? TRUE : FALSE;
}

BOOL WINAPI PE_Client_GetServerConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID lpOutBuffer, DWORD dwOutBufferSize, LPDWORD lpBytesReturned)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return FALSE;

    PE_CLIENT_SADPINFO sadp_info;
    if (!SelectLPPE_CLIENT_SADPINFOByUserIDAndChannel(lUserID, lChannel, &sadp_info))
        return FALSE;

    return lppe_client->lpclient->GetServerConfig(sadp_info.sUUID, dwCommand, lpOutBuffer, dwOutBufferSize, lpBytesReturned) ? TRUE : FALSE;
}

BOOL WINAPI PE_Client_PTZControl(LONG lRealHandle, DWORD dwPTZCommand, BOOL dwStop, DWORD dwSpeed)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByRealHandle(lRealHandle, &lppe_client))
        return FALSE;

    LPPE_CLIENT_PREVIEW lppe_preview = NULL;
    if (!SelectLPPE_CLIENT_PREVIEWByRealHandle(lRealHandle, &lppe_preview))
        return FALSE;

    return lppe_client->lpclient->ControlPtzWithSpeed(lppe_preview->sadp_info.sUUID,
        FALSE == dwStop ? dwPTZCommand : PE_PTZ_ALLSTOP, 
        dwSpeed)?TRUE:FALSE;
}

BOOL WINAPI PE_Client_GetSadpInfoList(LONG lUserID, LPPE_CLIENT_SADPINFO_LIST lpSadpInfoList)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return FALSE;

    int error_code = lppe_client->lpclient->GetSadpInfoList(lpSadpInfoList);
    if (error_code == PE_ERRCODE_SUCCESS){
        return TRUE;
    }
    else{
        g_api_last_error_code = error_code;
        return FALSE;
    }
}

LONG WINAPI PE_Client_GetFileByName(LONG lUserID, CONST CHAR *sServerFileName, CONST CHAR *sSavedFileDirectory)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_DOWNLOAD lppe_download = NULL;
    for (int i = 0; i < PE_MAX_DOWNLOAD; ++i){
        if (g_pe_download[i].unused){
            lppe_download = g_pe_download + i;
            break;
        }
    }
    if (NULL == lppe_download){
        g_api_last_error_code = PE_ERRCODE_OUTOFMEM;
        return -1;
    }

    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lUserID, &lppe_client))
        return -1;

    std::string filename(sServerFileName);
    std::string serverfilename;
    std::string outfilename;
    const std::string protocol_bin("BIN://");
    if (0 == filename.find(protocol_bin))
    {
        serverfilename = "C:\\Program Files\\PanoEye\\param\\";
        serverfilename += filename.substr(protocol_bin.length());
        outfilename = std::string(sSavedFileDirectory) + filename.substr(protocol_bin.length());
    }
    else
    {
        serverfilename = filename;
        auto pos = filename.find_last_of('\\');
        outfilename = std::string(sSavedFileDirectory) + 
            (pos == std::string::npos ? filename : filename.substr(pos+1));
    }

    lppe_download->dwRecvLength = 0;
    lppe_download->lUserID = lUserID;

    lppe_download->sFileName = outfilename;
    outfilename +=".tmp";
    FILE *fp=NULL;
    fp = fopen(outfilename.c_str(), "wb+");
    if (fp == NULL){
        g_api_last_error_code = PE_ERRCODE_FAIL;
        return -1;
    }
    lppe_download->fp = fp;

    DWORD dwFileSize;
    int handle;
    if (!lppe_client->lpclient->GetFileByName(serverfilename.c_str(), 0, &dwFileSize, &handle)){
        fclose(fp);
        g_api_last_error_code = PE_ERRCODE_FAIL;
        return -1;
    }

    lppe_download->dwFileSize = dwFileSize;
    lppe_download->nHandle = handle;
    lppe_download->unused = false;
    return lppe_download->lFileHandle;
}

int WINAPI PE_Client_GetDownloadPos(LONG lFileHandle)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_DOWNLOAD lppe_download = NULL;
    if (!SelectLPPE_CLIENT_DOWNLOADByFileHandle(lFileHandle, &lppe_download))
        return -1;

    if (lppe_download->dwRecvLength == lppe_download->dwFileSize)
        return 100;

    float fpos = lppe_download->dwRecvLength * 100.0 / lppe_download->dwFileSize;
    int ipos = fpos;
    if (ipos >= 100)
        return 99;

    return ipos;
}

BOOL WINAPI PE_Client_StopGetFile(LONG lFileHandle)
{
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    LPPE_CLIENT_DOWNLOAD lppe_download = NULL;
    if (!SelectLPPE_CLIENT_DOWNLOADByFileHandle(lFileHandle, &lppe_download))
        return FALSE;

    LPPE_CLIENT lppe_client = NULL;
    if (!SelectLPPE_CLIENTByUserID(lppe_download->lUserID, &lppe_client))
        return FALSE;

    int nHandle = lppe_download->nHandle;
    lppe_download->unused = true;

    lppe_client->lpclient->StopGetFile(nHandle);
    fclose(lppe_download->fp);

    rename((lppe_download->sFileName + ".tmp").c_str(), (lppe_download->sFileName).c_str());
    return TRUE;
}

LONG WINAPI PE_Client_GetLastError(){
    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    long tmp = g_api_last_error_code;
    g_api_last_error_code = PE_ERRCODE_SUCCESS;
    return tmp;
}

BOOL WINAPI PE_Client_Demux(char *pszData, long lDataLength, LPPE_AVFRAME_DATA *lpData){
    const size_t head_size = sizeof(PE_AVFRAME_DATA);
    if (NULL != pszData && lDataLength > head_size){
        LPPE_AVFRAME_DATA lpAVFrameData = LPPE_AVFRAME_DATA(pszData);
        lpAVFrameData->pszData = pszData + head_size;
        if (lpAVFrameData->lDataLength + head_size == lDataLength){
            *lpData = lpAVFrameData;
            return TRUE;
        }
    }
    return FALSE;
}

#if (defined(WIN32) || defined(_WIN32_WCE))
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        OutputDebugStringA("PEClientSDK V0.0.1 (201609091345)\n");
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#elif defined(__linux__) || defined(__APPLE__) //linux

#endif
