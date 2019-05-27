//////////////////////////////////////////////////////////////////////////
///     COPYRIGHT NOTICE
///     Copyright (c) 2013-2020, 宁波环视信息科技有限公司
///     All rights reserved.
///
/// @file       PEClientSDK.h
/// @brief      PanoEye PEClientSDK
///
/// @version    0.0.1
/// @author     Linjian Jiang
/// @created    2016/08/16
///
//////////////////////////////////////////////////////////////////////////

#ifndef PECLIENTSDK_PECLIENTSDK_H_
#define PECLIENTSDK_PECLIENTSDK_H_
#include "../type.h"
#if (defined(WIN32) || defined(_WIN32_WCE))
#include <Windows.h>
#ifdef PECLIENTSDKDLL_EXPORTS
#define PE_API __declspec(dllexport)
#else
#define PE_API __declspec(dllimport)
#endif
#ifndef WINAPI
#define WINAPI __stdcall
#endif
typedef unsigned __int64     UINT64;
typedef __int64              INT64;
#ifndef _TM_DEFINED
struct tm {
    int tm_sec;     /* seconds after the minute - [0,59] */
    int tm_min;     /* minutes after the hour - [0,59] */
    int tm_hour;    /* hours since midnight - [0,23] */
    int tm_mday;    /* day of the month - [1,31] */
    int tm_mon;     /* months since January - [0,11] */
    int tm_year;    /* years since 1900 */
    int tm_wday;    /* days since Sunday - [0,6] */
    int tm_yday;    /* days since January 1 - [0,365] */
    int tm_isdst;   /* daylight savings time flag */
};
#define _TM_DEFINED
#endif  /* _TM_DEFINED */
#elif defined(ANDROID_NDK)
#include "basetsd.h"
#include <time.h>
#define PE_API
#define CONST const
#define CALLBACK
#define WINAPI
#define IN
#define OUT
#elif defined(__linux__) || defined(__APPLE__) //linux
#ifdef PECLIENTSDKDLL_EXPORTS
#define PE_API __attribute__((visibility("default")))
#else
#define PE_API
#endif
#define CONST const
#define CALLBACK __stdcall
#define WINAPI __stdcall
#define IN
#define OUT
//typedef unsigned long long	 UINT64;
//typedef long long            INT64;
#endif
//#define PECLIENTSDKDLL_API PE_API
//#include "../type.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////////
//							错误代码
//////////////////////////////////////////////////////////////////////////
#define  PE_ERRCODE_BEGIN                                 0x8000F000
#define  PE_ERRCODE_SUCCESS                               (                    0)  //未知事件	
#define  PE_ERRCODE_FAIL                                  (PE_ERRCODE_BEGIN +  1)  //执行失败
#define  PE_ERRCODE_NOSUPPORT                             (PE_ERRCODE_BEGIN +  3)  //不支持该功能
#define  PE_ERRCODE_VERIFY                                (PE_ERRCODE_BEGIN +  4)  //验证错误
#define  PE_ERRCODE_OUTOFMEM                              (PE_ERRCODE_BEGIN +  5)  //内存溢出
#define  PE_ERRCODE_NOT_FIND                              (PE_ERRCODE_BEGIN +  7)  //未找到
#define  PE_ERRCODE_NULLPOINT                             (PE_ERRCODE_BEGIN +  9)  //空指针
#define  PE_ERRCODE_PARAM                                 (PE_ERRCODE_BEGIN + 10)  //参数错误
#define  PE_ERRCODE_USER_PWD                              (PE_ERRCODE_BEGIN + 11)  //用户名或密码错误
#define  PE_ERRCODE_NETWORK                               (PE_ERRCODE_BEGIN + 12)  //设备不在线或网络错误
#define  PE_ERRCODE_TIMEOUT                               (PE_ERRCODE_BEGIN + 13)  //网络超时

//////////////////////////////////////////////////////////////////////////
//							事件ID
//////////////////////////////////////////////////////////////////////////
#define  PE_EVENT_BEGIN                                   100
#define  PE_EVENT_UNKNOWN                                 (                   -1)  //未知事件
#define  PE_EVENT_LOGIN_CLOSE                             (PE_EVENT_BEGIN   +  1)  //登录连接被断开
#define  PE_EVENT_PREVIEW_CLOSE                           (PE_EVENT_BEGIN   +  2)  //预览连接被断开

#ifndef STRUCT_PE_ENCODER_TYPE_DEFINED
#define STRUCT_PE_ENCODER_TYPE_DEFINED 1
typedef enum tag_PE_ENCODER_TYPE
{
    PE_ENCODER_TYPE_UNKNOWN = 0,                        //未知编码
    PE_ENCODER_TYPE_H264 = 1,                           //H264编码
    PE_ENCODER_TYPE_MJPEG = 2,                          //MJPEG编码
    PE_ENCODER_TYPE_H264_MAIN = 3,                      //H264 main profile
    PE_ENCODER_TYPE_H264_HIGH = 4,                      //H264 high profile
    PE_ENCODER_TYPE_G711_ALAW = 5,                      //G711A律编码
    PE_ENCODER_TYPE_G711_ULAW = 6,                      //G711U律编码
    PE_ENCODER_TYPE_RAW_PCM = 7,                        //PCM编码，即不编码
}PE_ENCODER_TYPE;
#endif

#ifndef STRUCT_PE_AVFRAME_DATA_DEFINED
#define STRUCT_PE_AVFRAME_DATA_DEFINED 1
//音视频帧
typedef struct tag_PE_AVFRAME_DATA
{
    BYTE           byStreamFormat;                                      //1表示原始流，2表示混合流
    BYTE           byESStreamType;                                      //原始流类型，1表示视频，2表示音频
    BYTE           byEncoderType;                                       //编码格式,其值请参看PE_ENCODER_TYPE枚举定义
    BYTE           byFrameType;                                         //数据帧类型,1表示I帧, 2表示P帧, 0表示未知类型
    WORD           wFrameRate;                                          //帧率
    WORD           wBitRate;                                            //当前码率
    BYTE           byChannel;                                           //通道
    DWORD          lSequenceId;                                         //数据帧序号
    CHAR          *pszData;                                             //数据
    DWORD          lDataLength;                                         //数据有效长度
    DWORD          lImageWidth;                                         //视频宽度
    DWORD          lImageHeight;                                        //视频高度
    INT64          lTimeStamp;                                          //数据采集时间戳，单位为微秒
}PE_AVFRAME_DATA, *LPPE_AVFRAME_DATA;
#endif

#ifndef STRUCT_PE_CLIENT_DEFINED
#define STRUCT_PE_CLIENT_DEFINED 1
//预览接口
typedef struct tagPE_CLIENT_PREVIEWINFO
{
    LONG           lChannel;                                            //通道号
    DWORD          dwStreamType;                                        // 码流类型，0-主码流，1-子码流
    DWORD          dwLinkMode;                                          // 0：TCP方式
    BYTE           byProtoType;                                         //应用层取流协议，0-私有协议
    BYTE           byRes[171];                                          //保留
}PE_CLIENT_PREVIEWINFO, *LPPE_CLIENT_PREVIEWINFO;

typedef struct tagPE_CLIENT_FILECOND
{
    LONG           lChannel;
    DWORD          dwIsLocked;
    struct tm      struStartTime;
    struct tm      struStopTime;
    BYTE           byRes2[104];                                         //保留
}PE_CLIENT_FILECOND, *LPPE_CLIENT_FILECOND;

//录象文件参数
typedef struct tagPE_CLIENT_FINDDATA
{
    CHAR           sFileName[250];                                      //文件名
    struct tm      struStartTime;                                       //文件的开始时间
    struct tm      struStopTime;                                        //文件的结束时间
    DWORD          dwFileSize;                                          //文件的大小
    BYTE           byLocked;                                            //1表示此文件已经被锁定,0表示正常的文件
    BYTE           byRes[41];                                           //保留
}PE_CLIENT_FINDDATA, *LPPE_CLIENT_FINDDATA;

// 按ID+时间回放结构体
typedef struct tagPE_CLIENT_PLAYBACKINFO
{
    LONG           lChannel;                                            //通道号
    DWORD          dwStreamType;                                        // 码流类型，0-主码流，1-子码流
    struct tm      struBeginTime;
    struct tm      struEndTime;
    BYTE           byRes[104];
}PE_CLIENT_PLAYBACKINFO, *LPPE_CLIENT_PLAYBACKINFO;

typedef struct tagPE_CLIENT_SADPINFO
{
    CHAR	       sIP[64];
    CHAR           sUUID[64];
    CHAR	       sDevName[256];
    CHAR	       sSerialNo[64];
    BYTE           byRes[104];                                          //保留
}PE_CLIENT_SADPINFO, *LPPE_CLIENT_SADPINFO;

typedef struct tagPE_CLIENT_SADPINFO_LIST
{
    WORD           wSadpNum;                                            // 搜索到设备数目
    BYTE           byRes[182];                                          // 保留字节
    PE_CLIENT_SADPINFO struSadpInfo[256];                               // 搜索
}PE_CLIENT_SADPINFO_LIST, *LPPE_CLIENT_SADPINFO_LIST;
#endif

#ifndef _PE_PTZ_POS_CFG_DEFINED
typedef struct tagPE_PTZ_POS_CFG
{
    float fPanPos;//水平参数
    float fTiltPos;//垂直参数
    float fZoomPos;//变倍参数
    BYTE  byRes[172];
}PE_PTZ_POS_CFG, *LPPE_PTZ_POS_CFG;
#define _PE_PTZ_POS_CFG_DEFINED
#endif

#ifndef _PE_PTZ_CMD_DEFINED
/**********************云台控制命令 begin***********************/
#define PE_IRISCLOSE       0x0102         /**< 光圈关 */
#define PE_IRISOPEN        0x0104         /**< 光圈开 */
#define PE_FOCUSNEAR       0x0202         /**< 近聚集 */
#define PE_FOCUSFAR        0x0204         /**< 远聚集 */
#define PE_ZOOMTELE        0x0302         /**< 缩小 */
#define PE_ZOOMWIDE        0x0304         /**< 放大 */
#define PE_TILTUP          0x0402         /**< 向上 */
#define PE_TILTDOWN        0x0404         /**< 向下 */
#define PE_PANRIGHT        0x0502         /**< 向右 */
#define PE_PANLEFT         0x0504         /**< 向左 */
#define PE_LEFTUP          0x0702         /**< 左上 */
#define PE_LEFTDOWN        0x0704         /**< 左下 */
#define PE_RIGHTUP         0x0802         /**< 右上 */
#define PE_RIGHTDOWN       0x0804         /**< 右下 */
#define PE_BRUSHON         0x0A01         /**< 雨刷开 */
#define PE_BRUSHOFF        0x0A02         /**< 雨刷关 */
#define PE_BRUSHONCE       0x0A03         /**< 雨刷单次 */
#define PE_LIGHTON         0x0B01         /**< 灯开 */
#define PE_LIGHTOFF        0x0B02         /**< 灯关 */
#define PE_INFRAREDON      0x0C01         /**< 红外开 */
#define PE_INFRAREDOFF     0x0C02         /**< 红外关 */
#define PE_HEATON          0x0D01         /**< 加热开 */
#define PE_HEATOFF         0x0D02         /**< 加热关 */
/**********************云台控制命令 end*************************/
#define _PE_PTZ_CMD_DEFINED
#endif

#ifndef _PE_PLAY_CMD_DEFINED
/**********************播放控制命令 begin***********************/
#define PE_PLAYPAUSE       0x001          /**< 暂停播放 */
#define PE_PLAYRESTART     0x002          /**< 恢复播放（将恢复暂停前的速度播放） */
#define PE_PLAYFAST        0x003          /**< 快放 */
#define PE_PLAYSLOW        0x004          /**< 慢放 */
#define PE_PLAYNORMAL      0x005          /**< 正常速度播放*/
#define PE_PLAYSETPOS      0x011          /**< 改变文件回放的进度 */
#define PE_PLAYGETPOS      0x012          /**< 获取按文件回放的进度 */
#define PE_PLAYSETTIME     0x014          /**< 按绝对时间定位 */
#define PE_PLAYGETTIME     0x013          /**< 获取当前已经播放的时间*/
/**********************播放控制命令 end*************************/
#define _PE_PLAY_CMD_DEFINED
#endif

#ifndef _PE_SERVER_CONFIG_CMD_DEFINED
/**********************服务器配置命令 begin*********************/
#define PE_SET_PTZ_POS     14
#define PE_GET_PTZ_POS     15
/**********************服务器配置命令 end***********************/
#define _PE_SERVER_CONFIG_CMD_DEFINED
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (CALLBACK *fPERealDataCallBack)(LONG lRealHandle, CONST CHAR * szSerialNo, int nStreamID, LPPE_AVFRAME_DATA lpData, void * pUserData);
typedef void (CALLBACK *fPEPlayDataCallBack)(LONG lPlayHandle, CONST CHAR *sServerFileName, int nStreamID, LPPE_AVFRAME_DATA lpData, void * pUserData);
typedef void (CALLBACK *fPEExceptionCallBack)(DWORD dwType, LONG lHandle, void *pUserData);

PE_API BOOL WINAPI PE_Client_Init();
PE_API BOOL WINAPI PE_Client_Cleanup();
PE_API BOOL WINAPI PE_Client_SetExceptionCallBack(fPEExceptionCallBack cbExceptionCallBack, void* pUserData);

PE_API LONG WINAPI PE_Client_Login(CONST CHAR *sDVRIP, WORD wDVRPort, CONST CHAR *sUserName, CONST CHAR *sPassword);
PE_API BOOL WINAPI PE_Client_Logout(LONG lUserID);
PE_API LONG WINAPI PE_Client_RealPlay(LONG lUserID, LPPE_CLIENT_PREVIEWINFO lpPreviewInfo, fPERealDataCallBack cbRealDataCallBack, void* pUserData);
PE_API BOOL WINAPI PE_Client_StopRealPlay(LONG lRealHandle);

PE_API LONG WINAPI PE_Client_FindFile(LONG lUserID, LPPE_CLIENT_FILECOND pFindCond);
PE_API BOOL WINAPI PE_Client_FindNextFile(LONG lFindHandle, LPPE_CLIENT_FINDDATA lpFindData);
PE_API BOOL WINAPI PE_Client_FindClose(LONG lFindHandle);

PE_API LONG WINAPI PE_Client_PlayBackByName(LONG lUserID, CONST CHAR *sPlayBackFileName, fPEPlayDataCallBack cbPlayDataCallBack, void* pUserData);
PE_API LONG WINAPI PE_Client_PlayBackByTime(LONG lUserID, CONST LPPE_CLIENT_PLAYBACKINFO para, fPEPlayDataCallBack cbPlayDataCallBack, void* pUserData);
PE_API BOOL WINAPI PE_Client_PlayBackControl(LONG lPlayHandle, DWORD dwControlCode, LPVOID lpInBuffer = NULL, DWORD dwInLen = 0, LPVOID lpOutBuffer = NULL, DWORD *lpOutLen = NULL);
PE_API BOOL WINAPI PE_Client_StopPlayBack(LONG lPlayHandle);

PE_API BOOL WINAPI PE_Client_SetServerConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID lpInBuffer, DWORD dwInBufferSize);
PE_API BOOL WINAPI PE_Client_GetServerConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID lpOutBuffer, DWORD dwOutBufferSize, LPDWORD lpBytesReturned);
PE_API BOOL WINAPI PE_Client_PTZControl(LONG lRealHandle, DWORD dwPTZCommand, BOOL dwStop, DWORD dwSpeed);

PE_API BOOL WINAPI PE_Client_GetSadpInfoList(LONG lUserID, LPPE_CLIENT_SADPINFO_LIST lpSadpInfoList);

PE_API LONG WINAPI PE_Client_GetFileByName(LONG lUserID, CONST CHAR *sServerFileName, CONST CHAR *sSavedFileDirectory);
PE_API int  WINAPI PE_Client_GetDownloadPos(LONG lFileHandle);
PE_API BOOL WINAPI PE_Client_StopGetFile(LONG lFileHandle);

PE_API LONG WINAPI PE_Client_GetLastError();
PE_API BOOL WINAPI PE_Client_Demux(char *pszData, long lDataLength, LPPE_AVFRAME_DATA *lpData);

#ifdef __cplusplus
}
#endif

#endif
