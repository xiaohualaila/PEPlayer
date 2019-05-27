//////////////////////////////////////////////////////////////////////////
///     COPYRIGHT NOTICE
///     Copyright (c) 2013-2020, ����������Ϣ�Ƽ����޹�˾
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
#elif defined(__linux__) || defined(__APPLE__) //linux
#ifdef PECLIENTSDKDLL_EXPORTS
#define PE_API __attribute__((visibility("default")))
#else
#define PE_API
#endif
#define WINAPI __stdcall
#define IN
#define OUT
#endif
//#define PECLIENTSDKDLL_API PE_API

#include "../type.h"
#include <time.h>
//#ifndef _TM_DEFINED
//struct tm {
//    int tm_sec;     /* seconds after the minute - [0,59] */
//    int tm_min;     /* minutes after the hour - [0,59] */
//    int tm_hour;    /* hours since midnight - [0,23] */
//    int tm_mday;    /* day of the month - [1,31] */
//    int tm_mon;     /* months since January - [0,11] */
//    int tm_year;    /* years since 1900 */
//    int tm_wday;    /* days since Sunday - [0,6] */
//    int tm_yday;    /* days since January 1 - [0,365] */
//    int tm_isdst;   /* daylight savings time flag */
//};
//#define _TM_DEFINED
//#endif  /* _TM_DEFINED */


//////////////////////////////////////////////////////////////////////////
//							�������
//////////////////////////////////////////////////////////////////////////
#define  PE_ERRCODE_BEGIN                                 0x8000F000
#define  PE_ERRCODE_SUCCESS                               (                    0)  //δ֪�¼�	
#define  PE_ERRCODE_FAIL                                  (PE_ERRCODE_BEGIN +  1)  //ִ��ʧ��
#define  PE_ERRCODE_NOSUPPORT                             (PE_ERRCODE_BEGIN +  3)  //��֧�ָù���
#define  PE_ERRCODE_VERIFY                                (PE_ERRCODE_BEGIN +  4)  //��֤����
#define  PE_ERRCODE_OUTOFMEM                              (PE_ERRCODE_BEGIN +  5)  //�ڴ����
#define  PE_ERRCODE_NOT_FIND                              (PE_ERRCODE_BEGIN +  7)  //δ�ҵ�
#define  PE_ERRCODE_NULLPOINT                             (PE_ERRCODE_BEGIN +  9)  //��ָ��
#define  PE_ERRCODE_PARAM                                 (PE_ERRCODE_BEGIN + 10)  //��������
#define  PE_ERRCODE_USER_PWD                              (PE_ERRCODE_BEGIN + 11)  //�û������������
#define  PE_ERRCODE_NETWORK                               (PE_ERRCODE_BEGIN + 12)  //�豸�����߻��������
#define  PE_ERRCODE_TIMEOUT                               (PE_ERRCODE_BEGIN + 13)
//////////////////////////////////////////////////////////////////////////
//							�¼�ID
//////////////////////////////////////////////////////////////////////////
#define  PE_EVENT_BEGIN                                   100
#define  PE_EVENT_UNKNOWN                                 (                   -1)  //δ֪�¼�
#define  PE_EVENT_LOGIN_CLOSE                             (PE_EVENT_BEGIN   +  1)  //��¼���ӱ��Ͽ�
#define  PE_EVENT_PREVIEW_CLOSE                           (PE_EVENT_BEGIN   +  2)  //Ԥ�����ӱ��Ͽ�

#ifndef STRUCT_PE_ENCODER_TYPE_DEFINED
#define STRUCT_PE_ENCODER_TYPE_DEFINED 1
typedef enum tag_PE_ENCODER_TYPE
{
    PE_ENCODER_TYPE_UNKNOWN = 0,                        //δ֪����
    PE_ENCODER_TYPE_H264 = 1,                           //H264����
    PE_ENCODER_TYPE_MJPEG = 2,                          //MJPEG����
    PE_ENCODER_TYPE_H264_MAIN = 3,                      //H264 main profile
    PE_ENCODER_TYPE_H264_HIGH = 4,                      //H264 high profile
    PE_ENCODER_TYPE_G711_ALAW = 5,                      //G711A�ɱ���
    PE_ENCODER_TYPE_G711_ULAW = 6,                      //G711U�ɱ���
    PE_ENCODER_TYPE_RAW_PCM = 7,                        //PCM���룬��������
}PE_ENCODER_TYPE;
#endif

#ifndef STRUCT_PE_AVFRAME_DATA_DEFINED
#define STRUCT_PE_AVFRAME_DATA_DEFINED 1
//����Ƶ֡
typedef struct tag_PE_AVFRAME_DATA
{
    BYTE           byStreamFormat;                                      //1��ʾԭʼ����2��ʾ�����
    BYTE           byESStreamType;                                      //ԭʼ�����ͣ�1��ʾ��Ƶ��2��ʾ��Ƶ
    BYTE           byEncoderType;                                       //�����ʽ,��ֵ��ο�PE_ENCODER_TYPEö�ٶ���
    BYTE           byFrameType;                                         //����֡����,1��ʾI֡, 2��ʾP֡, 0��ʾδ֪����
    WORD           wFrameRate;                                          //֡��
    WORD           wBitRate;                                            //��ǰ����
    BYTE           byChannel;                                           //ͨ��
    DWORD          lSequenceId;                                         //����֡���
    CHAR          *pszData;                                             //����
    DWORD          lDataLength;                                         //������Ч����
    DWORD          lImageWidth;                                         //��Ƶ���
    DWORD          lImageHeight;                                        //��Ƶ�߶�
    INT64          lTimeStamp;                                          //���ݲɼ�ʱ�������λΪ΢��
}PE_AVFRAME_DATA, *LPPE_AVFRAME_DATA;
#endif

#ifndef STRUCT_PE_CLIENT_DEFINED
#define STRUCT_PE_CLIENT_DEFINED 1
//Ԥ���ӿ�
typedef struct tagPE_CLIENT_PREVIEWINFO
{
    LONG           lChannel;                                            //ͨ����
    DWORD          dwStreamType;                                        // �������ͣ�0-��������1-������
    DWORD          dwLinkMode;                                          // 0��TCP��ʽ
    BYTE           byProtoType;                                         //Ӧ�ò�ȡ��Э�飬0-˽��Э��
    BYTE           byRes[171];                                          //����
}PE_CLIENT_PREVIEWINFO, *LPPE_CLIENT_PREVIEWINFO;

typedef struct tagPE_CLIENT_FILECOND
{
    LONG           lChannel;
    DWORD          dwIsLocked;
    struct tm      struStartTime;
    struct tm      struStopTime;
    BYTE           byRes2[104];                                         //����
}PE_CLIENT_FILECOND, *LPPE_CLIENT_FILECOND;

//¼���ļ�����
typedef struct tagPE_CLIENT_FINDDATA
{
    CHAR           sFileName[250];                                      //�ļ���
    struct tm      struStartTime;                                       //�ļ��Ŀ�ʼʱ��
    struct tm      struStopTime;                                        //�ļ��Ľ���ʱ��
    DWORD          dwFileSize;                                          //�ļ��Ĵ�С
    BYTE           byLocked;                                            //1��ʾ���ļ��Ѿ�������,0��ʾ�������ļ�
    BYTE           byRes[41];                                           //����
}PE_CLIENT_FINDDATA, *LPPE_CLIENT_FINDDATA;

typedef struct tagPE_CLIENT_SADPINFO
{
    CHAR	       sIP[64];
    CHAR           sUUID[64];
    CHAR	       sDevName[256];
    CHAR	       sSerialNo[64];
    BYTE           byRes[104];                                          //����
}PE_CLIENT_SADPINFO, *LPPE_CLIENT_SADPINFO;

typedef struct tagPE_CLIENT_SADPINFO_LIST
{
    WORD           wSadpNum;                                            // �������豸��Ŀ
    BYTE           byRes[182];                                          // �����ֽ�
    PE_CLIENT_SADPINFO struSadpInfo[256];                               // ����
}PE_CLIENT_SADPINFO_LIST, *LPPE_CLIENT_SADPINFO_LIST;
#endif

#ifndef _PE_PTZ_POS_CFG_DEFINED
typedef struct tagPE_PTZ_POS_CFG
{
    float fPanPos;//ˮƽ����
    float fTiltPos;//��ֱ����
    float fZoomPos;//�䱶����
    BYTE  byRes[172];
}PE_PTZ_POS_CFG, *LPPE_PTZ_POS_CFG;
#define _PE_PTZ_POS_CFG_DEFINED
#endif

#ifndef _PE_PTZ_CMD_DEFINED
/**********************��̨�������� begin***********************/
#define PE_IRISCLOSE       0x0102         /**< ��Ȧ�� */
#define PE_IRISOPEN        0x0104         /**< ��Ȧ�� */
#define PE_FOCUSNEAR       0x0202         /**< ���ۼ� */
#define PE_FOCUSFAR        0x0204         /**< Զ�ۼ� */
#define PE_ZOOMTELE        0x0302         /**< ��С */
#define PE_ZOOMWIDE        0x0304         /**< �Ŵ� */
#define PE_TILTUP          0x0402         /**< ���� */
#define PE_TILTDOWN        0x0404         /**< ���� */
#define PE_PANRIGHT        0x0502         /**< ���� */
#define PE_PANLEFT         0x0504         /**< ���� */
#define PE_LEFTUP          0x0702         /**< ���� */
#define PE_LEFTDOWN        0x0704         /**< ���� */
#define PE_RIGHTUP         0x0802         /**< ���� */
#define PE_RIGHTDOWN       0x0804         /**< ���� */
#define PE_BRUSHON         0x0A01         /**< ��ˢ�� */
#define PE_BRUSHOFF        0x0A02         /**< ��ˢ�� */
#define PE_LIGHTON         0x0B01         /**< �ƿ� */
#define PE_LIGHTOFF        0x0B02         /**< �ƹ� */
#define PE_INFRAREDON      0x0C01         /**< ���⿪ */
#define PE_INFRAREDOFF     0x0C02         /**< ����� */
#define PE_HEATON          0x0D01         /**< ���ȿ� */
#define PE_HEATOFF         0x0D02         /**< ���ȹ� */
/**********************��̨�������� end*************************/
#define _PE_PTZ_CMD_DEFINED
#endif

#ifndef _PE_PLAY_CMD_DEFINED
/**********************���ſ������� begin***********************/
#define PE_PLAYPAUSE       0x001          /**< ��ͣ���� */
#define PE_PLAYRESTART     0x002          /**< �ָ����ţ����ָ���ͣǰ���ٶȲ��ţ� */
#define PE_PLAYFAST        0x003          /**< ��� */
#define PE_PLAYSLOW        0x004          /**< ���� */
#define PE_PLAYNORMAL      0x005          /**< �����ٶȲ���*/
#define PE_PLAYSETPOS      0x011          /**< �ı��ļ��طŵĽ��� */
#define PE_PLAYGETPOS      0x012          /**< ��ȡ���ļ��طŵĽ��� */
#define PE_PLAYGETTIME     0x013          /**< ��ȡ��ǰ�Ѿ����ŵ�ʱ��*/
#define PE_PLAYSETTIME     0x014          /**< ������ʱ�䶨λ */
/**********************���ſ������� end*************************/
#define _PE_PLAY_CMD_DEFINED
#endif

#ifndef _PE_SERVER_CONFIG_CMD_DEFINED
/**********************�������������� begin*********************/
#define PE_SET_PTZ_POS     14
#define PE_GET_PTZ_POS     15
/**********************�������������� end***********************/
#define _PE_SERVER_CONFIG_CMD_DEFINED
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CONST const
#define CALLBACK __stdcall
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
