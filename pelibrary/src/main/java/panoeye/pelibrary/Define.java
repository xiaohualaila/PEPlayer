package panoeye.pelibrary;

import java.util.Date;

/**
 * Created by tir on 2016/12/8.
 */

public class Define {
    public static int cameraCount = 8;
    static int CUR_WIDTH = 1280;
    static int CUR_HEIGHT = 960;
    static Integer GAIN_WIDTH = 320;
    static Integer GAIN_HEIGHT = 240;
    public static boolean isCmbExist = false;
    public static int pointCount = 1;
    static Integer CIF_ROW = 288;
    static Integer CIF_COL = 352;
    static Integer D1_ROW = 576;
    static Integer D1_COL = 704;
    static Integer QCIF_ROW = 144;
    static Integer QCIF_COL = 176;
    static Integer QVGA_ROW = 240;
    static Integer QVGA_COL = 320;

    static Integer PE_B = 1;
    static Integer PE_M = 12;

    static Integer SIZE_INT = 4;
    static Integer SIZE_CHAR = 1;
    static Integer SIZE_FLOAT =4;
    static Integer SIZE_CAMERA = 84;

    static Integer SIZE_PES_HEAD = 2000;
    static Integer SIZE_PER_HEAD = 432;
    static Integer order_PER_serial_no = 295;
    static Integer order_PER_bin_zip_order = 359;
    static Integer order_PER_bin_zip_size = 363;
    static Integer order_PER_index_list_order = 420;

    static Integer CAMCOUNT_MIN = 2;
    static Integer CAMCOUNT_MAX = 8;

    public static float x = 0;
    public static float y = 0;
    public static float z = 0;
    public static float fovy = 45;
    public static int openglFrameRate = 0;
    public static int videoFrameRate = 0;

    public static boolean flip = false;
    public static boolean isNeedFilp = false;
    public static int mode = 0; //0 全景  1 vr  2 原始
    public static final int MODE_GLOBULAR = 0;
    public static final int MODE_VR = 1;
    public static final int MODE_ORIGINAL = 2;
    public static boolean showVR = false;

    public static String ROOT_PATH = "/storage/emulated/0/Pano/";
    public static String PARAM_PATH = "/storage/emulated/0/Pano/param/";

    public static boolean isStretch = false;
    public static boolean isHalfBall = true;
    public static boolean isInBall = false;
    public static boolean isFlip = false;
    public static float xRatio = 1.0f;//1.0f
    public static float yRatio = 1.0f;//1.0f
    public static float xCircle = 0.5f;//0.5f
    public static float yCircle = 0.5f;//0.5f
}

//java没有结构体，故class当struct用
class PER_FILE_Head{
    //    byte magic_head[20];//"PanoEye Record PEFile\0"
    byte file_format_version;//0x01,第一版
    int stream_payload_order;//有效载荷，有效负荷，(有效信息起始位)的绝对偏移量(起点）
    int stream_payload_size; //有效负载大小(单位为字节数)
    byte packet_type;			//保留，'P':=媒体格式（动态负载大小）,'T':=传输格式（还未制定，固定188个字节传输包）
    int dev_info_order;		//配置信息的绝对偏移量
    int record_info_order;		//录像信息的绝对偏移量

    PER_FILE_Head_DevInfo dev_info;//配置结构体
    PER_FILE_Head_RecordInfo record_info;//录像结构体
}
class PER_FILE_Head_DevInfo{
    byte magic_number; //0x40
    //    byte dev_name[256];//相机名称//E1078
//    byte serial_no[64];//相机序列号//E10010078
    int bin_zip_order;	//bin压缩文件的绝对偏移量(起点）
    int bin_zip_size;  //bin压缩文件大小
    int  stream_mode;	//保留，不用//0
}

class PER_FILE_Head_RecordInfo{
    byte magic_number; //0x41
    Date create_time;      //录像创建时间
    Date record_start_time;//录像开始时间
    Date record_end_time;  //录像结束时间
    int  index_list_order;		//PER_IndexIter结构体列表的绝对偏移量(起点）
    int  index_list_size;		//PER_IndexIter结构体列表的数量
    int  index_list_iter_size;	//PER_IndexIter结构体的尺寸
}

class PER_PS_PacketHead
{
    int  sync_byte;		//同步字节，总是0x000001BB，表示这个包是正确的
    int PES_Packet_Order;	//距离这个头的偏移( 起点）
}
class PER_PES_PacketHead
{
    byte[] sync_byte = new byte[3];		//0x000001
    byte stream_id;		//数据来源，相机ID和类型，Examples: Audio streams (0xD0-0xDE), Video Main Streams (0xE0-0xEE), Video Sub Streams (0xB0-0xBE), 视频编辑数据 (0xC0-0xCE),最多15个通道。 0xXF,作为使用扩展通道
    int     PE_Packet_Order;	//距离这个头的偏移( 起点）
}
class PER_PE_Video_PacketHead
{
    int sync_byte;		//0x000001B3
    Date tPTS;				//UTS绝对时间，显示时间
    float    fPTS;				//相对于文件开始时间（秒为单位）
    Date    tDTS;				//UTS绝对时间，解码I-Frame时间
    float    fDTS;				//相对于文件开始时间
    int     RAW_Data_Order;	//距离这个头的偏移( 起点）
    int     RAW_Data_Size;
    //扩展区
    byte is_key;		//关键帧
    int frame_index;	//当前帧ID
    int keyframe_index; //当前帧依赖的关键帧ID
}
class PER_IndexIter{
    byte magic_number;
    int pre_index_order;
    int next_index_order;
    int stream_packet_order;
    Date cur_time;
    float cPDt;
    byte is_key;
    byte stream_id;
    int frame_index;
    int keyframe_index;
}


