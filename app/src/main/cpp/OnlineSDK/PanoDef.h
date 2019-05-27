#ifndef _PANO_DEF_H_
#define _PANO_DEF_H_
#include <limits.h>
#include "type.h"
#ifdef WIN32
#ifndef WINVER                  // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef WINVER  
#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif
#endif

//#define USE_ENG_UI
#ifndef USE_ENG_UI
#define USE_CHN_UI
#endif

//for SDK export
//#define PANOEYE_SDK
#define USE_OCULUS

#ifndef PANOEYE_SDK

	//define if using H3C camera���������
	//#define USE_H3C_CAM
	#ifdef USE_H3C_CAM
		//#define USE_H3C_IPC_OLD
		#define USE_H3C_IPC_NEW
	#endif

	//���������������/ 2013/11/28 songkk
	#define USE_HK_CAM
	#ifdef USE_HK_CAM
	#define USE_SR_CAM  // �������
	#define USE_CY_CAM  // �������
	#endif

	//#define USE_KD_CAM
	//#define USE_HT_DECODE
	//�󻪸����������/ 2013/11/28 songkk
	#define USE_DH_CAM

	//�ǰ������������/ 2014/4/23 jianglj
	#define USE_YA_CAM
    #ifdef USE_YA_CAM
    //#define USE_YA_CAM_INTERFACE
    #endif

#endif


//define if Box projection
#define USE_BOX_PROJECTION

#define PANO_MAX_CHANNEL_NUM	8
#define PANO_MAX_NET_IMG_NUM	100

#define PANO_CLIENT_PROC_NUM			4
#define PANO_CLIENT_CNUM_PER_PROC		8
#define PANO_SERVER_PROC_NUM			1
#define PANO_SERVER_CNUM_PER_PROC		32
//#define PANO_LINK_PROC_NUM				3
//#define PANO_LINK_CNUM_PER_PROC			10

#define RestartTimeGap 1.5

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

enum Disp_List 
{
	List_None,			//0  
	List_Inited,		//1
	List_ReInit,		//2
};

enum Display_Mode 
{
	Cam360_PTZ,			//0
	Cam360_Plane,		//1
	Cam360_Cylinder,	//2
	Cam360_Origin,		//3
	Cam360_Fusion,		//4
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

enum Net_Modes
{
	Net_No_Connect,
	Net_Connecting,
	Net_Connected,
};

enum Sensor_Model
{
	PS_XM  =0,		// ����
	PS_TST =1,		// ����ͨ
	PS_H3C =2,		// ����
	PS_HK  =3,		// �����������
	PS_DH  =4,		// �����
	PS_YA  =5,	    // �ǰ����
	PS_HT  =6,	    // ��;����
	PS_NXP =7,	    // NXP����(����ʱ��)
	PS_SR  =8,      // �������
	PS_CY  =9,      // �������
	PS_KD = 10,      // �ƴ����
};

enum Stream_Format
{
	PS_MAIN,
	PS_EXTRA,
};

enum PE_Model
{
	PE_A=0,		//0: 4-ch, ���壺A0: 1280x960; A1:2048x1536(3M)
	PE_B,		//1: 8-ch, ���壺704x576;
	PE_C,		//2: 2-ch, ���壺C0: 1280x960;
	PE_D,		//3: 5-ch, ���壺D0: 1280x960;
	PE_E,		//4: 8-ch, ���壺E0,E1: 1280x960; E3,E4:2048x1536(3M)
	PE_F,		//5: 7-ch, ���壺F0: 1280x960;
	PE_G,		//6: 3-ch, ���壺G0: 1280x960, ˮƽ; G1��1280x960,��ֱ
	PE_H,		//7: 6-ch, ���壺H0: 1920x1280(2M, TST); H1: (3M, NXP)
	PE_L = 11,	//11: 7-ch, ���峵�� LA��1280x720
	PE_M = 12,	//12: 8-ch, ���峵�أ�M0: 704-576;���峵�� MA��1280x720
	PE_N,		//13: 6-ch, ���峵�أ�N0: 1280x960;
	PE_O,		//14: No use
	PE_P,		//15: 4-ch, �������˲t����P0: 1280x960;
	PE_Q,		//16: 6-ch, �������˲t����P0: 2048x1536;
	PE_R,		//17: 4-ch, ��·ˮƽ��R0: 1280x720;
	PE_S,		//17: 6-ch, 6·ˮƽ��S0: 2048x1536;
	PE_Z=25,	//��������룺1-ch; 
				//  Z0��������Z1������ͨ��Z2������ 
				//  Z3��������Z4���󻪣�Z5:�ǰ��� 
				//	Z6:��;��Z7:�������
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
	CLIENT_START,
	CLIENT_STOP,
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

	PE_PTZ_IRISCLOSESTOP = 0x0101,			/**< ��Ȧ��ֹͣ */
	PE_PTZ_IRISCLOSE = 0x0102,				/**< ��Ȧ�� */
	PE_PTZ_IRISOPENSTOP = 0x0103,			/**< ��Ȧ��ֹͣ */
	PE_PTZ_IRISOPEN = 0x0104,				/**< ��Ȧ�� */

	PE_PTZ_FOCUSNEARSTOP = 0x0201,			/**< ���ۼ�ֹͣ */
	PE_PTZ_FOCUSNEAR = 0x0202,				/**< ���ۼ� */
	PE_PTZ_FOCUSFARSTOP = 0x0203,			/**< Զ�ۼ� ֹͣ*/
	PE_PTZ_FOCUSFAR = 0x0204,				/**< Զ�ۼ� */

	PE_PTZ_ZOOMTELESTOP = 0x0301,			/**< ��Сֹͣ */
	PE_PTZ_ZOOMTELE = 0x0302,				/**< ��С */
	PE_PTZ_ZOOMWIDESTOP = 0x0303,			/**< �Ŵ�ֹͣ */
	PE_PTZ_ZOOMWIDE = 0x0304,				/**< �Ŵ� */

	PE_PTZ_TILTUPSTOP = 0x0401,				/**< ����ֹͣ */
	PE_PTZ_TILTUP = 0x0402,					/**< ���� */
	PE_PTZ_TILTDOWNSTOP = 0x0403,			/**< ����ֹͣ */
	PE_PTZ_TILTDOWN = 0x0404,				/**< ���� */

	PE_PTZ_PANRIGHTSTOP = 0x0501,			/**< ����ֹͣ */
	PE_PTZ_PANRIGHT = 0x0502,				/**< ���� */
	PE_PTZ_PANLEFTSTOP = 0x0503,			/**< ����ֹͣ */
	PE_PTZ_PANLEFT = 0x0504,				/**< ���� */

	PE_PTZ_PRESAVE = 0x0601,				/**< Ԥ��λ���� */
	PE_PTZ_PRECALL = 0x0602,				/**< Ԥ��λ���� */
	PE_PTZ_PREDEL = 0x0603,					/**< Ԥ��λɾ�� */
		
	PE_PTZ_LEFTUPSTOP = 0x0701,				/**< ����ֹͣ */
	PE_PTZ_LEFTUP = 0x0702,					/**< ���� */
	PE_PTZ_LEFTDOWNSTOP = 0x0703,			/**< ����ֹͣ */
	PE_PTZ_LEFTDOWN = 0x0704,				/**< ���� */

	PE_PTZ_RIGHTUPSTOP = 0x0801,			/**< ����ֹͣ */
	PE_PTZ_RIGHTUP = 0x0802,				/**< ���� */
	PE_PTZ_RIGHTDOWNSTOP = 0x0803,			/**< ����ֹͣ */
	PE_PTZ_RIGHTDOWN = 0x0804,				/**< ���� */

	PE_PTZ_ALLSTOP = 0x0901,				/**< ȫͣ������ */

	PE_PTZ_BRUSHON = 0x0A01,				/**< ��ˢ�� */
	PE_PTZ_BRUSHOFF = 0x0A02,				/**< ��ˢ�� */

	PE_PTZ_LIGHTON = 0x0B01,				/**< �ƿ� */
	PE_PTZ_LIGHTOFF = 0x0B02,				/**< �ƹ� */

	PE_PTZ_INFRAREDON = 0x0C01,				/**< ���⿪ */
	PE_PTZ_INFRAREDOFF = 0x0C02,			/**< ����� */

	PE_PTZ_HEATON = 0x0D01,					/**< ���ȿ� */
	PE_PTZ_HEATOFF = 0x0D02,				/**< ���ȹ� */

	PE_PTZ_SCANCRUISE = 0x0E01,				/**< ��̨����ɨè */
	PE_PTZ_SCANCRUISESTOP = 0x0E02,			/**< ��̨����ɨè */

	PE_PTZ_TRACKCRUISE = 0x0F01,			/**<  ��̨�켣Ѳ�� */
	PE_PTZ_TRACKCRUISESTOP = 0x0F02,		/**<  ��̨�켣Ѳ�� */

	PE_PTZ_PRESETCRUISE = 0x1001,			/**<  ��̨��Ԥ��λѲ�� ���������ֲ�����̨ģ������ */
	PE_PTZ_PRESETCRUISESTOP = 0x1002,		/**<  ��̨��Ԥ��λѲ�� ֹͣ���������ֲ�����̨ģ������ */
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
	//float x[3], y[3];
	float tx[3],ty[3];
	float bx[3],by[3],bz[3];		//box position
	float sx[3],sy[3],sz[3];		//sphere position
	unsigned char on;
};

typedef unsigned long ULONG;
typedef unsigned char uchar;

///////////////////////////////////////////////////////////
struct PanoComm
{
	char	keyword[4];			//"PECM"
	UINT	command;			//see struct Comm_Type
	int		pkgSize;			//total packet size including data part
};

void PanoComm_Init(struct PanoComm *);
bool PanoComm_Check(struct PanoComm *);
unsigned int PanoComm_Size();

struct PE_StreamZeroChanData
{
	int		camId;				//camera id
    int     stream_format;      //stream format
};

//#include <winsock2.h>			//to prevent <winsock.h> in <windows.h>
//#include <windows.h>

#ifndef _PE_CONNECT_CFG_DEFINED
typedef struct tagPE_CONNECT_CFG
{
    BYTE  byNetState;
    BYTE  byStopConnect;
    BYTE  byAutoConnect;
    BYTE  bySubStreamOnly;
    BYTE  byRes[180];
}PE_CONNECT_CFG, *LPPE_CONNECT_CFG;
#define _PE_CONNECT_CFG_DEFINED
#endif

#ifndef _PE_PTZ_POS_CFG_DEFINED
typedef struct tagPE_PTZ_POS_CFG
{
    float fPanPos; //水平参数
    float fTiltPos;//垂直参数
    float fZoomPos;//变倍参数
    BYTE  byRes[172];
}PE_PTZ_POS_CFG, *LPPE_PTZ_POS_CFG;
#define _PE_PTZ_POS_CFG_DEFINED
#endif

#ifndef _PE_POINT_FRAME_DEFINED
typedef struct tagPE_POINT_FRAME
{
    float  xTop;     //������ʼ���x����
    float  yTop;     //����������y����
    float  xBottom;  //����������x����
    float  yBottom;  //����������y����
    float  bCounter; //����
    BYTE   byRes[164];
}PE_POINT_FRAME, *LPPE_POINT_FRAME;
#define _PE_POINT_FRAME_DEFINED
#endif

#ifndef _PE_STREAM_DATA_DEFINED
typedef struct tagPE_STREAM_DATA
{
    DWORD    dwSize;
    BYTE     byPreviewID;
    BYTE     byNID; //netimg ID
    BYTE     byPID;	//camera id
    BYTE     byStreamFormat;      //stream format
    INT64    llTimeStamp;
    FLOAT    fTimeInterval;
    BYTE     byFrameType;
    DWORD    lImageWidth;
    DWORD    lImageHeight;
}PE_STREAM_DATA, *LPPE_STREAM_DATA;
#define _PE_STREAM_DATA_DEFINED
#endif

#ifndef _PE_FILE_DATA_DEFINED
typedef struct tagPE_FILE_DATA
{
    DWORD    dwSize;
    BYTE     byPreviewID;
    DWORD    dwFileSize;
    DWORD    dwOffset;
    DWORD    dwDataLength;
}PE_FILE_DATA, *LPPE_FILE_DATA;
#define _PE_FILE_DATA_DEFINED
#endif

//#if !defined(USING_HPSOCKET)
//#define USING_HPSOCKET
//#endif

#endif
