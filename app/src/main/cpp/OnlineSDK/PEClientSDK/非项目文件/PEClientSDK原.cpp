#if (defined(WIN32) || defined(_WIN32_WCE))
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#elif defined(ANDROID_NDK)
#include "basetsd.h"
#elif defined(__linux__) || defined(__APPLE__) //linux
#include "../CrossPlatformCDefine/CrossPlatformCDefine.h"
#endif
#include "PEClientSDK.h"
#include "PEClient.h"
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

#if defined(ANDROID_NDK)
using std::min;
using std::max;
#endif

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

typedef struct tag_PE_CLIENT_PLAYBACK_BY_FILE  PE_CLIENT_PLAYBACK_BY_FILE;
typedef struct tag_PE_CLIENT_PLAYBACK_BY_FILE *LPPE_CLIENT_PLAYBACK_BY_FILE;
typedef struct tag_PE_CLIENT_PLAYBACK_BY_TIME  PE_CLIENT_PLAYBACK_BY_TIME;
typedef struct tag_PE_CLIENT_PLAYBACK_BY_TIME *LPPE_CLIENT_PLAYBACK_BY_TIME;

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

struct tag_PE_CLIENT_PLAYBACK_BY_FILE{
    PE_CLIENT_FINDDATA find_data;
};

struct tag_PE_CLIENT_PLAYBACK_BY_TIME{
    struct tm		      struBeginTime;
    struct tm		      struEndTime;
};

struct tag_PE_CLIENT_PLAYBACK{
    bool unused;
    PE_CLIENT_PREVIEWINFO preview_info;
    PE_CLIENT_SADPINFO sadp_info;
    LONG lUserID;
    LONG lPlayHandle;
    INT  nHandle;
    INT  play_mode; //0: File, 1: Time
    tag_PE_CLIENT_PLAYBACK_BY_FILE struByFile;
    tag_PE_CLIENT_PLAYBACK_BY_TIME struByTime;
    fPEPlayDataCallBack cbPlayDataCallBack;
    void* pUserData;
    DWORD lSequenceId[256];

    INT64 lCurTimeStamp;
    INT64 lBeginTimeStamp;
    INT64 lEndTimeStamp;
    int play_speed;
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
static std::recursive_mutex                 g_api_recursive_mutex;

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

void PEThrowException(CONST CPEClient *client, DWORD dwType)
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

    std::lock_guard<std::recursive_mutex> lck(g_api_recursive_mutex);
    if (g_exception_callback)
        g_exception_callback(dwType, lUserID, g_exception_userdata);
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
    data->pszData = (CHAR *)buff;
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

    lppe_playback->lCurTimeStamp = lpInfo->llTimeStamp;

    fPEPlayDataCallBack cbPlayDataCallBack = lppe_playback->cbPlayDataCallBack;
    if (NULL == cbPlayDataCallBack)
        return true;

    PE_AVFRAME_DATA data;
    //Struct_PE_AVFRAME_DATA_Init(lpInfo, pData, iLength, &data);
    //data.lSequenceId = lppe_preview->lSequenceId++;
    Struct_PE_AVFRAME_DATA_InitMux(lpInfo, pData, iLength, &data);
    LPPE_AVFRAME_DATA(data.pszData)->lSequenceId = lppe_playback->lSequenceId[lpInfo->byPID]++;

    cbPlayDataCallBack(lPlayHandle,
        lppe_playback->struByFile.find_data.sFileName, lppe_playback->preview_info.dwStreamType,
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
    int error_code = lpclient->Login((char *)sDVRIP, wDVRPort, (char *)sUserName, (char *)sPassword);
    if (error_code != PE_ERRCODE_SUCCESS){
        delete lpclient;
        g_api_last_error_code = error_code;
        return -1;
    }
    else{
        lppe_client->lpclient = lpclient;
        strcpy(lppe_client->sDVRIP, (char *)sDVRIP);
        lppe_client->wDVRPort = wDVRPort;
        strcpy(lppe_client->sUserName, (char *)sUserName);
        strcpy(lppe_client->sPassword, (char *)sPassword);
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

    if (lpPreviewInfo->byProtoType == 0 && lpPreviewInfo->dwLinkMode == 0)
    {
        lppe_preview->lUserID = lUserID;
        memcpy(&(lppe_preview->preview_info), lpPreviewInfo, sizeof(*lpPreviewInfo));
        memcpy(&(lppe_preview->sadp_info), &sadp_info, sizeof(sadp_info));
        lppe_preview->cbRealDataCallBack = cbRealDataCallBack;
        lppe_preview->pUserData = pUserData;
        for (int j = 0; j < 256; ++j)
            lppe_preview->lSequenceId[j] = 0;

        int handle;
        if (lppe_client->lpclient->RequestStreamData((char*)sadp_info.sUUID, lpPreviewInfo->dwStreamType, &handle)){
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

    lppe_client->lpclient->StopRealPlay(nHandle);

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
    if (!lppe_client->lpclient->GetRecordFileList((char*)sadp_info.sUUID,
		begTime, endTime, &info,1000))
	{
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
            strcpy((char *)find_data->sFileName, info[i].full_dir.c_str());       //文件名
            ConvertStringToStructTm(info[i].record_time.c_str(), &find_data->struStartTime);//文件的开始时间
            ConvertStringToStructTm(info[i].end_time.c_str(), &find_data->struStopTime);//文件的结束时间                                    
            find_data->dwFileSize=0;                                        //文件的大小
            find_data->byLocked=0;                                          //1表示此文件已经被锁定,0表示正常的文件
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
                    if (0 == strcmp((char*)find_data.sFileName, sPlayBackFileName)){
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
    if (!FindPlayBackInfoByName(lUserID, (char *)sPlayBackFileName, &preview_info, &sadp_info, &find_data))
        return -1;
    
    memcpy(&lppe_playback->preview_info, &preview_info, sizeof(preview_info));
    memcpy(&lppe_playback->sadp_info, &sadp_info, sizeof(sadp_info));
    memcpy(&lppe_playback->struByFile.find_data, &find_data, sizeof(find_data));
    lppe_playback->play_mode = 0;
    lppe_playback->lUserID = lUserID;
    lppe_playback->cbPlayDataCallBack = cbPlayDataCallBack;
    lppe_playback->pUserData = pUserData;
    for (int j = 0; j < 256; ++j)
        lppe_playback->lSequenceId[j] = 0;

    float total_time;
    struct tm recTime;
    int handle;
    if (!lppe_client->lpclient->PlayRecordFile((char *)sadp_info.sUUID, (char *)sPlayBackFileName, &total_time, &recTime, &handle)){
        g_api_last_error_code = PE_ERRCODE_FAIL;
        return -1;
    }

    time_t t = mktime(&recTime);
    lppe_playback->lBeginTimeStamp = t * 1000 * 1000;
    lppe_playback->lCurTimeStamp = lppe_playback->lBeginTimeStamp;
    lppe_playback->lEndTimeStamp = lppe_playback->lBeginTimeStamp + total_time*1000*1000;
    lppe_playback->nHandle = handle;
    lppe_playback->unused = false;
    return lppe_playback->lPlayHandle;
}

LONG WINAPI PE_Client_PlayBackByTime(LONG lUserID, CONST LPPE_CLIENT_PLAYBACKINFO para, fPEPlayDataCallBack cbPlayDataCallBack, void* pUserData)
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
    preview_info.lChannel=para->lChannel;
    preview_info.dwStreamType=para->dwStreamType;
    preview_info.dwLinkMode=0;
    preview_info.byProtoType=0;
    PE_CLIENT_SADPINFO sadp_info;
    if (!SelectLPPE_CLIENT_SADPINFOByUserIDAndChannel(lUserID, preview_info.lChannel, &sadp_info))
        return -1;


    memcpy(&lppe_playback->preview_info, &preview_info, sizeof(preview_info));
    memcpy(&lppe_playback->sadp_info, &sadp_info, sizeof(sadp_info));
    memcpy(&(lppe_playback->struByTime.struBeginTime), &(para->struBeginTime), sizeof(struct tm));
    memcpy(&(lppe_playback->struByTime.struEndTime), &(para->struEndTime), sizeof(struct tm));
    lppe_playback->play_mode = 1;
    lppe_playback->lUserID = lUserID;
    lppe_playback->cbPlayDataCallBack = cbPlayDataCallBack;
    lppe_playback->pUserData = pUserData;
    for (int j = 0; j < 256; ++j)
        lppe_playback->lSequenceId[j] = 0;

    int handle;
    if (!lppe_client->lpclient->PlayRecordByTime((char *)sadp_info.sUUID, para->struBeginTime, para->struEndTime, para->dwStreamType, &handle)){
        g_api_last_error_code = PE_ERRCODE_FAIL;
        return -1;
    }

    time_t t = mktime(&(para->struBeginTime));
    time_t t2 = mktime(&(para->struEndTime));
    lppe_playback->lBeginTimeStamp = t * 1000 * 1000;
    lppe_playback->lCurTimeStamp = lppe_playback->lBeginTimeStamp;
    lppe_playback->lEndTimeStamp = t2 * 1000 * 1000;
    lppe_playback->nHandle = handle;
    lppe_playback->unused = false;
    return lppe_playback->lPlayHandle;
}

static bool PE_Client_PlayBackControl_By_File(LPPE_CLIENT lppe_client, LPPE_CLIENT_PLAYBACK lppe_playback, DWORD dwControlCode, LPVOID lpInBuffer, DWORD dwInLen, LPVOID lpOutBuffer, DWORD *lpOutLen)
{
    switch (dwControlCode){
    case PE_PLAYPAUSE: lppe_client->lpclient->PausePlayRecord(lppe_playback->nHandle, true);  break;
    case PE_PLAYRESTART: lppe_client->lpclient->PausePlayRecord(lppe_playback->nHandle, false);  break;
    case PE_PLAYFAST:
        ++lppe_playback->play_speed;
        lppe_playback->play_speed = min(lppe_playback->play_speed, 4);
        return lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYSLOW:
        --lppe_playback->play_speed;
        lppe_playback->play_speed = max(lppe_playback->play_speed, -4);
        return lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYNORMAL:
        lppe_playback->play_speed = 0;
        return lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYSETPOS:
    {
        float in = *(float*)lpInBuffer;
        in = min(in, 99.0f);
        in = max(in, 0.0f);
        float cur_tm = in * (lppe_playback->lEndTimeStamp - lppe_playback->lBeginTimeStamp) / 1000 / 1000;
        return lppe_client->lpclient->SetPlayRecordCurrentTime(lppe_playback->nHandle, cur_tm);
    }
    break;
    case PE_PLAYGETPOS:
    {
        *(float *)lpOutBuffer = (lppe_playback->lCurTimeStamp - lppe_playback->lBeginTimeStamp)
            / (lppe_playback->lEndTimeStamp - lppe_playback->lBeginTimeStamp);
        *lpOutLen = 4;
        return true;
    }
    break;
    case PE_PLAYGETTIME:
    {
        time_t t = lppe_playback->lCurTimeStamp / 1000 / 1000;
        struct tm ct = *localtime(&t);
        memcpy(lpOutBuffer, &ct, sizeof(ct));
        *lpOutLen = sizeof(ct);
        return true;
    }
    break;
    case PE_PLAYSETTIME:
    {
        struct tm *ct = (struct tm*)lpInBuffer;
        time_t ct1 = mktime(ct);
        INT64 lCurTimeStamp = ct1 * 1000 * 1000;
        lCurTimeStamp = min(lCurTimeStamp, lppe_playback->lEndTimeStamp);
        lCurTimeStamp = max(lCurTimeStamp, lppe_playback->lBeginTimeStamp);
        float cur_tm = (lCurTimeStamp - lppe_playback->lBeginTimeStamp) / 1000 / 1000;
        return lppe_client->lpclient->SetPlayRecordCurrentTime(lppe_playback->nHandle, cur_tm);
    }
    break;
    }
    return false;
}

static bool PE_Client_PlayBackControl_By_Time(LPPE_CLIENT lppe_client, LPPE_CLIENT_PLAYBACK lppe_playback, DWORD dwControlCode, LPVOID lpInBuffer, DWORD dwInLen, LPVOID lpOutBuffer, DWORD *lpOutLen)
{
    switch (dwControlCode){
    case PE_PLAYPAUSE: lppe_client->lpclient->PausePlayRecord(lppe_playback->nHandle, true);  break;
    case PE_PLAYRESTART: lppe_client->lpclient->PausePlayRecord(lppe_playback->nHandle, false);  break;
    case PE_PLAYFAST:
        ++lppe_playback->play_speed;
        lppe_playback->play_speed = min(lppe_playback->play_speed, 4);
        return lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYSLOW:
        --lppe_playback->play_speed;
        lppe_playback->play_speed = max(lppe_playback->play_speed, -4);
        return lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYNORMAL:
        lppe_playback->play_speed = 0;
        return lppe_client->lpclient->SetPlayRecordSpeed(lppe_playback->nHandle, lppe_playback->play_speed);
        break;
    case PE_PLAYGETPOS:
    {
        *(float *)lpOutBuffer = (lppe_playback->lCurTimeStamp - lppe_playback->lBeginTimeStamp)
            / (lppe_playback->lEndTimeStamp - lppe_playback->lBeginTimeStamp);
        *lpOutLen = 4;
        return true;
    }
    break;
    case PE_PLAYGETTIME:
    {
        time_t t = lppe_playback->lCurTimeStamp / 1000 / 1000;
        struct tm ct = *localtime(&t);
        memcpy(lpOutBuffer, &ct, sizeof(ct));
        *lpOutLen = sizeof(ct);
        return true;
    }
    break;
    }
    return false;
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
    
    if (lppe_playback->play_mode == 0){
        ret = PE_Client_PlayBackControl_By_File(lppe_client, lppe_playback, dwControlCode, lpInBuffer, dwInLen, lpOutBuffer, lpOutLen);
    }
    else if (lppe_playback->play_mode == 1){
        ret = PE_Client_PlayBackControl_By_Time(lppe_client, lppe_playback, dwControlCode, lpInBuffer, dwInLen, lpOutBuffer, lpOutLen);
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
    
    return lppe_client->lpclient->SetServerConfig((char*)sadp_info.sUUID, dwCommand, lpInBuffer, dwInBufferSize) ? TRUE : FALSE;
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

    return lppe_client->lpclient->GetServerConfig((char*)sadp_info.sUUID, dwCommand, lpOutBuffer, dwOutBufferSize, lpBytesReturned) ? TRUE : FALSE;
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

    return lppe_client->lpclient->ControlPtzWithSpeed((char*)lppe_preview->sadp_info.sUUID,
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

    std::string filename((char*)sServerFileName);
    std::string serverfilename;
    std::string outfilename;
    const std::string protocol_bin("BIN://");
    if (0 == filename.find(protocol_bin))
    {
        serverfilename = "C:\\Program Files\\PanoEye\\param\\";
        serverfilename += filename.substr(protocol_bin.length());
        outfilename = std::string((char*)sSavedFileDirectory) + filename.substr(protocol_bin.length());
    }
    else
    {
        serverfilename = filename;
        auto pos = filename.find_last_of('\\');
        outfilename = std::string((char*)sSavedFileDirectory) +
            (pos == std::string::npos ? filename : filename.substr(pos+1));
    }

    lppe_download->sFileName = outfilename;
    lppe_download->dwRecvLength = 0;
    lppe_download->lUserID = lUserID;
    
    outfilename += ".tmp";
    FILE *fp = fopen(outfilename.c_str(), "wb+");
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
        lpAVFrameData->pszData = (CHAR *)pszData + head_size;
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
