#ifndef _PANO_DEF_H_
#define _PANO_DEF_H_
#include <limits.h>

#if !defined(WIN32)
#define WIN32
#endif
#if !defined(_WINDOWS)
#define _WINDOWS
#endif

#if defined(WIN32)

#ifndef WINVER                  // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef WINVER  
#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif

#include <winsock2.h>			//to prevent <winsock.h> in <windows.h>
#include <windows.h>
#include <basetsd.h>


typedef unsigned long ULONG;
typedef unsigned char uchar;

#if !defined(USING_HPSOCKET)
#define USING_HPSOCKET
#endif

#elif defined(ANDROID_NDK)
#include "basetsd.h"
#endif

//#define USE_ENG_UI
#ifndef USE_ENG_UI
#define USE_CHN_UI
#endif

//for SDK export
//#define PANOEYE_SDK
#define useHtcVive
#define useFullTexture true

#ifndef PANOEYE_SDK

	//define if using H3C camera，宇视相机
	//#define USE_H3C_CAM
	#ifdef USE_H3C_CAM
		//#define USE_H3C_IPC_OLD
		#define USE_H3C_IPC_NEW
	#endif

	//#define USE_XM_DECODE
	//#define USE_TST_DECODE
	//海康高速智能球机/ 2013/11/28 songkk
	#define USE_HK_CAM
	#ifdef USE_HK_CAM
	#define USE_SR_CAM  // 数尔球机
	#define USE_CY_CAM  // 城宇球机
	#endif

	#define USE_KD_CAM
	#define USE_KD_CAM_NEW
	//#define USE_HT_DECODE
	//大华高速智能球机/ 2013/11/28 songkk
	#define USE_DH_CAM

	//亚安高速智能球机/ 2014/4/23 jianglj
	#define USE_YA_CAM
    #ifdef USE_YA_CAM
    //#define USE_YA_CAM_INTERFACE
    #endif

#endif

typedef unsigned long ULONG;
typedef unsigned char uchar;

//define if Box projection
#define USE_BOX_PROJECTION
#define USING_PANO_REPLAY_PORT			1

#define PANO_NET_COMM					1

#define PANO_MAX_CHANNEL_NUM			8		//每台全景的接入相机的最大单元数量
#define PANO_MAX_NET_IMG_NUM			100

#define PANO_CLIENT_PROC_NUM			5		//客户端解码子进程数量
#define PANO_CLIENT_PROC_NUM_MAIN		1		//客户端解码主进程数量
#define PANO_CLIENT_PROC_NUM_REPLAY		1		//客户端REPLAY进程数量
#define PANO_CLIENT_PROC_NUM_MEM		1		//客户端MEM进程数量
#define PANO_CLIENT_CNUM_PER_PROC		8		//每个客户端解码子进程接入相机数量

#define PANO_NET_MAX_PROC_NUM			200		//服务器端子进程数量
#define PANO_NET_MAX_COMM_NUM			20		//服务器端子进程数量
//#define PANO_NET_PROC_NUM				60		//服务器端子进程数量
//#define PANO_NET_PROC_NUM_KD			1		//服务器端KD子进程数量
//#define PANO_NET_PROC_NUM_COMM		1		//服务器端通讯子进程数量
#define PANO_NET_PROC_NUM_MEM			1		//服务器端MEM子进程数量
#define PANO_NET_PROC_NUM_CONNECT		1		//服务器端MEM子进程数量
#define PANO_NET_CNUM_PER_PROC			PANO_MAX_CHANNEL_NUM		//每个服务器端子进程接入相机数量
//#define PANO_NET_CNUM_PER_PROC_KD		8		//每个服务器端子进程接入相机数量

//#define PANO_SERVER_PROC_NUM			1		//服务器端子进程数量
//#define PANO_SERVER_CNUM_PER_PROC		32		//每个服务器端子进程接入相机数量

#define PANO_MAX_LOADED_CAMERA			(PANO_CLIENT_PROC_NUM*PANO_CLIENT_CNUM_PER_PROC)		//动态加载相机，如果大于这个数量就卸载这个相机
#define PANO_TOTAL_CAMERA				1000	//系统接入相机总数量

#define RestartTimeGap					1.5		//如果进程失去联系，重启进程的时间间隔

////////////////////////////////////
#define CIF_ROW		288
#define CIF_COL		352

#define D1_ROW		576
#define D1_COL		704

#define QCIF_ROW	144
#define QCIF_COL	176

#define QVGA_ROW	240
#define QVGA_COL	320

//#define COLOR_GAIN_ROW	288			//240
//#define COLOR_GAIN_COL	352			//320

#define COLOR_GAIN_ROW	240			//240
#define COLOR_GAIN_COL	320			//320

#define STD_Radius	450.0f			//for Sphere radius

#define ZipBinfileSize (5*1024*1024)

///////////////////////////////////////////////////////
enum PE_LANGUAGE
{
	PE_CHINESE = 0,
	PE_ENGLISH = 1,
	PE_JAPANESE = 2,
};
//enum Disp_List 
//{
//	List_None,			//0  
//	List_Inited,		//1
//	List_ReInit,		//2
//};
//
//enum Display_Mode 
//{
//	Cam360_PTZ,			//0
//	Cam360_Plane,		//1
//	Cam360_Cylinder,	//2
//	Cam360_Origin,		//3
//	Cam360_Fusion,		//4
//};

enum CODEC_TYPE
{
	ENCODER_TYPE_UNKNOWN = 0,						//未知编码
	ENCODER_TYPE_H264 = 1,							//H264编码
	ENCODER_TYPE_MJPEG = 2,							//MJPEG编码
	ENCODER_TYPE_H264_MAIN = 3,						//H264 main profile
	ENCODER_TYPE_H264_HIGH = 4,						//H264 high profile
	ENCODER_TYPE_G711_ALAW = 5,						//G711A律编码
	ENCODER_TYPE_G711_ULAW = 6,						//G711U律编码
	ENCODER_TYPE_RAW_PCM = 7,						//PCM编码，即不编码
	ENCODER_TYPE_H265 = 8,							//H265编码
};

enum Cam360_Resolution 
{
	Cam360_D1,			//0
	Cam360_CIF,			//1
};

enum RecordMode
{
	Record_D1,			//0
	Record_CIF,			//1
	Record_DVR          //2
};

enum System_Modes
{
	Sys_None,
	Sys_Image,
	Sys_Net_Data,
};

enum Replay_Types
{
	Replay_None,
	Replay_PES,
	Replay_AVI,
	Replay_PER,
};

enum Camera_Close_Modes
{
	Cam_Close_All,
	Cam_Close_Pm_Release,
	Cam_Close_Pm_Keep,
};

enum Camera_Modes
{
	Cam_NotReady,
	Cam_Ready,
	Cam_UnLoad,
	Cam_Loading,
	Cam_Loaded,
};

enum Net_Modes
{
	Net_No_Connect,
	Net_Connecting,
	Net_Connected,
};

#define RSTP_BIAS 100
enum Sensor_Model	//等于	sub_model_type;
{
	PESM_MODEL_NONE =-2,	// 已被占用
	PESM_MODEL_ALL =-1,		// 所有模型
	PESM_XM  =0,			// 雄迈
	PESM_TST =1,			// 天视通
	PESM_YS  =2,			// 宇视
	PESM_HK  =3,			// 海康高速球机
	PESM_DH  =4,			// 大华球机
	PESM_YA  =5,			// 亚安球机
	PESM_HT  =6,			// 华途数字
	PESM_NXP =7,			// NXP方案(华创时代)
	PESM_SR  =8,			// 数尔球机
	PESM_CY  =9,			// 城宇球机
	PESM_KD = 10,			// 科达球机

	RTSP_XM = 11,	// 雄迈
	RTSP_TST = 12,			// 天视通
	RTSP_YS = 13,			// 宇视
	RTSP_HK = 14,			// 海康高速球机
	RTSP_DH = 15,			// 大华球机
	RTSP_YA = 16,			// 亚安球机
	RTSP_HT = 17,			// 华途数字
	RTSP_NXP = 18,			// NXP方案(华创时代)
	RTSP_SR = 19,			// 数尔球机
	RTSP_CY = 20,			// 城宇球机
	RTSP_KD = 21,			// 科达球机
};

enum Stream_Format
{
	PESM_MAIN_STREAM,
	PESM_SUB_STREAM,
};

enum PE_Model
{
	PE_A=0,		//0: 4-ch, 高清：A0: 1280x960; A1:2048x1536(3M)
	PE_B,		//1: 8-ch, 标清：704x576;
	PE_C,		//2: 2-ch, 高清：C0: 1280x960;
	PE_D,		//3: 5-ch, 高清：D0: 1280x960;
	PE_E,		//4: 8-ch, 高清：E0,E1: 1280x960; E3,E4:2048x1536(3M)
	PE_F,		//5: 7-ch, 高清：F0: 1280x960;
	PE_G,		//6: 3-ch, 高清：G0: 1280x960, 水平; G1：1280x960,垂直
	PE_H,		//7: 6-ch, 高清：H0: 1920x1280(2M, TST); H1: (3M, NXP)
	PE_L = 11,	//11: 7-ch, 高清车载 LA：1280x720
	PE_M = 12,	//12: 8-ch, 标清车载：M0: 704-576;高清车载 MA：1280x720
	PE_N,		//13: 6-ch, 高清车载：N0: 1280x960;
	PE_O,		//14: No use
	PE_P,		//15: 4-ch, 高清立杆瞭望：P0: 1280x960;
	PE_Q,		//16: 6-ch, 高清立杆瞭望：P0: 2048x1536;
	PE_R,		//17: 4-ch, 四路水平：R0: 1280x720;
	PE_S,		//17: 6-ch, 6路水平：S0: 2048x1536;
	PE_Z=25,	//单相机接入：1-ch; 
				//  Z0：雄迈，Z1：天视通，Z2：宇视 
				//  Z3：海康，Z4：大华，Z5:亚安， 
				//	Z6:华途，Z7:数尔球机
};

enum Video_Type
{
	VT_AVI,
	VT_H264,
	VT_PES,
	VT_PER,
};

enum Play_Mode 
{
	Video_Play   =0,		//0
	Video_Pause  =1,		//1
	Video_Stop   =2,		//2
	Video_Finish =3,		//3
	Video_Close  =4,		//4
	Video_Open   =5,		//5
};

enum Play_DVRMode 
{
	ByName,				//0
	ByTime,				//1
};


enum Comm_Type
{
    CT_STREAM_ZERO_CHAN_DATA = 1,
    CT_STREAM_ZERO_CHAN_AUDIO_DATA = 2,
    CT_STREAM_ZERO_CHAN_SYS_HEAD = 3,
    CT_STREAM_ZERO_CHAN_TIME_SYNC = 4,
    CT_XMLRPC_OVER_PES_REQUEST = 5,
    CT_XMLRPC_OVER_PES_RESPONSE = 6,
    CT_SERVER_VERSION_REQ = 7,
    CT_SERVER_VERSION_SEND = 8,
    CT_EMPTY = 9,
    CT_FILE_TRANSLATE = 10,
    CT_SET_ZERO_CHAN_CFG = 11,
    CT_SET_CONNECT_CFG = 12,
    CT_GET_CONNECT_CFG = 13,
    CT_SET_PTZ_POS = 14,
    CT_GET_PTZ_POS = 15,
    CT_SET_PTZ_SEL_ZOOM_IN = 16,
    CT_STREAM_DATA = 17,
    CT_STREAM_AUDIO_DATA = 18,
    CT_STREAM_SYS_HEAD = 19,
};

enum Server_State
{
	SERVER_START,
	SERVER_STOP,
	CLIENT_CONNECT,
	CLIENT_DISCONNECT,
};

enum PE_PTZ_MOUSE_MODE
{
	PE_PTZ_MOUSE_STOP = -1,
	PE_PTZ_MOUSE_DOWN = 0,
	PE_PTZ_MOUSE_UP = 1,
	PE_PTZ_MOUSE_RIGHT = 2,
	PE_PTZ_MOUSE_LEFT = 3,
	PE_PTZ_MOUSE_ZOOM_IN = 4,
	PE_PTZ_MOUSE_ZOOM_OUT = 5,
	PE_PTZ_MOUSE_FOLLOW = 6,
};

enum PE_PTZ_CMD
{
	PE_PTZ_CMD_MOUSE = 0,
	PE_PTZ_CMD_BUTTON = 1,
	PE_PTZ_CMD_MOVE = 2,
	PE_PTZ_CMD_3D_MOVE = 3,
	PE_PTZ_CMD_STOP = 4,
	PE_PTZ_CMD_GET_INFO = 5,
	PE_PTZ_CMD_SET_SPEED = 6,

	PE_PTZ_IRISCLOSESTOP = 0x0101,			/**< 光圈关停止 */
	PE_PTZ_IRISCLOSE = 0x0102,				/**< 光圈关 */
	PE_PTZ_IRISOPENSTOP = 0x0103,			/**< 光圈开停止 */
	PE_PTZ_IRISOPEN = 0x0104,				/**< 光圈开 */

	PE_PTZ_FOCUSNEARSTOP = 0x0201,			/**< 近聚集停止 */
	PE_PTZ_FOCUSNEAR = 0x0202,				/**< 近聚集 */
	PE_PTZ_FOCUSFARSTOP = 0x0203,			/**< 远聚集 停止*/
	PE_PTZ_FOCUSFAR = 0x0204,				/**< 远聚集 */

	PE_PTZ_ZOOMTELESTOP = 0x0301,			/**< 缩小停止 */
	PE_PTZ_ZOOMTELE = 0x0302,				/**< 缩小 */
	PE_PTZ_ZOOMWIDESTOP = 0x0303,			/**< 放大停止 */
	PE_PTZ_ZOOMWIDE = 0x0304,				/**< 放大 */

	PE_PTZ_TILTUPSTOP = 0x0401,				/**< 向上停止 */
	PE_PTZ_TILTUP = 0x0402,					/**< 向上 */
	PE_PTZ_TILTDOWNSTOP = 0x0403,			/**< 向下停止 */
	PE_PTZ_TILTDOWN = 0x0404,				/**< 向下 */

	PE_PTZ_PANRIGHTSTOP = 0x0501,			/**< 向右停止 */
	PE_PTZ_PANRIGHT = 0x0502,				/**< 向右 */
	PE_PTZ_PANLEFTSTOP = 0x0503,			/**< 向左停止 */
	PE_PTZ_PANLEFT = 0x0504,				/**< 向左 */

	PE_PTZ_PRESAVE = 0x0601,				/**< 预置位保存 */
	PE_PTZ_PRECALL = 0x0602,				/**< 预置位调用 */
	PE_PTZ_PREDEL = 0x0603,					/**< 预置位删除 */
		
	PE_PTZ_LEFTUPSTOP = 0x0701,				/**< 左上停止 */
	PE_PTZ_LEFTUP = 0x0702,					/**< 左上 */
	PE_PTZ_LEFTDOWNSTOP = 0x0703,			/**< 左下停止 */
	PE_PTZ_LEFTDOWN = 0x0704,				/**< 左下 */

	PE_PTZ_RIGHTUPSTOP = 0x0801,			/**< 右上停止 */
	PE_PTZ_RIGHTUP = 0x0802,				/**< 右上 */
	PE_PTZ_RIGHTDOWNSTOP = 0x0803,			/**< 右下停止 */
	PE_PTZ_RIGHTDOWN = 0x0804,				/**< 右下 */

	PE_PTZ_ALLSTOP = 0x0901,				/**< 全停命令字 */

	PE_PTZ_BRUSHON = 0x0A01,				/**< 雨刷开 */
	PE_PTZ_BRUSHOFF = 0x0A02,				/**< 雨刷关 */
	PE_PTZ_BRUSHONCE = 0x0A03,				/**< 雨刷单次 */

	PE_PTZ_LIGHTON = 0x0B01,				/**< 灯开 */
	PE_PTZ_LIGHTOFF = 0x0B02,				/**< 灯关 */

	PE_PTZ_INFRAREDON = 0x0C01,				/**< 红外开 */
	PE_PTZ_INFRAREDOFF = 0x0C02,			/**< 红外关 */

	PE_PTZ_HEATON = 0x0D01,					/**< 加热开 */
	PE_PTZ_HEATOFF = 0x0D02,				/**< 加热关 */

	PE_PTZ_SCANCRUISE = 0x0E01,				/**< 云台线性扫猫 */
	PE_PTZ_SCANCRUISESTOP = 0x0E02,			/**< 云台线性扫猫 */

	PE_PTZ_TRACKCRUISE = 0x0F01,			/**<  云台轨迹巡航 */
	PE_PTZ_TRACKCRUISESTOP = 0x0F02,		/**<  云台轨迹巡航 */

	PE_PTZ_PRESETCRUISE = 0x1001,			/**<  云台按预置位巡航 ，该命令字不在云台模板体现 */
	PE_PTZ_PRESETCRUISESTOP = 0x1002,		/**<  云台按预置位巡航 停止，该命令字不在云台模板体现 */
};


////////////////////////////////////////////////
struct PanoTri
{
	float x[3], y[3];				//cylinder position
	float tx[3],ty[3];				//texture position
	float sx[3],sy[3],sz[3];		//sphere position
};

struct PanoTriTexture
{
	float tx[3], ty[3];				//texture position
};

struct PanoBoxLine
{
	float bx[2],by[2],bz[2];		//box position
	float sx[2],sy[2],sz[2];		//sphere position
};

struct PanoBoxTri
{
	float tx[3],ty[3];
	float bx[3],by[3],bz[3];		//box position
	float sx[3],sy[3],sz[3];		//sphere position
	unsigned char on;
};

struct PanoMesh
{
	float tx, ty;
	float x, y, z;				//plane original position
};

///////////////////////////////////////////////////////////
struct PanoComm
{
    INT8    keyword[4];			//"PECM"
    UINT32  command;			//see struct Comm_Type
    INT32   pkgSize;			//total packet size including data part
};

void PanoComm_Init(struct PanoComm *);
bool PanoComm_Check(struct PanoComm *);
unsigned int PanoComm_Size();

struct PE_StreamZeroChanData
{
    INT32  camId;				//camera id
    INT32  stream_format;      //stream format
};

#ifndef _PE_CONNECT_CFG_DEFINED
typedef struct tagPE_CONNECT_CFG
{
    UINT8  byNetState;
    UINT8  byStopConnect;
    UINT8  byAutoConnect;
    UINT8  bySubStreamOnly;
    UINT8  byRes[180];
}PE_CONNECT_CFG, *LPPE_CONNECT_CFG;
#define _PE_CONNECT_CFG_DEFINED
#endif

#ifndef _PE_PTZ_POS_CFG_DEFINED
typedef struct tagPE_PTZ_POS_CFG
{
    float fPanPos;//水平参数
    float fTiltPos;//垂直参数
    float fZoomPos;//变倍参数
    UINT8 byRes[172];
}PE_PTZ_POS_CFG, *LPPE_PTZ_POS_CFG;
#define _PE_PTZ_POS_CFG_DEFINED
#endif

#ifndef _PE_POINT_FRAME_DEFINED
typedef struct tagPE_POINT_FRAME
{
    float  xTop;     //方框起始点的x坐标
    float  yTop;     //方框结束点的y坐标
    float  xBottom;  //方框结束点的x坐标
    float  yBottom;  //方框结束点的y坐标
    float  bCounter; //保留
    UINT8  byRes[164];
}PE_POINT_FRAME, *LPPE_POINT_FRAME;
#define _PE_POINT_FRAME_DEFINED
#endif

#ifndef _PE_STREAM_DATA_DEFINED
typedef struct tagPE_STREAM_DATA
{
    DWORD32  dwSize;
    UINT8    byPreviewID;
    UINT8    byNID; //netimg ID
    UINT8    byPID;	//camera id
    UINT8    byStreamFormat;      //stream format
    INT64    llTimeStamp;
    FLOAT    fTimeInterval;
    UINT8    byFrameType;
    DWORD32  lImageWidth;
    DWORD32  lImageHeight;
}PE_STREAM_DATA, *LPPE_STREAM_DATA;
#define _PE_STREAM_DATA_DEFINED
#endif

#ifndef _PE_FILE_DATA_DEFINED
typedef struct tagPE_FILE_DATA
{
    DWORD32  dwSize;
    UINT8    byPreviewID;
    DWORD32  dwFileSize;
    DWORD32  dwOffset;
    DWORD32  dwDataLength;
}PE_FILE_DATA, *LPPE_FILE_DATA;
#define _PE_FILE_DATA_DEFINED
#endif

#endif
