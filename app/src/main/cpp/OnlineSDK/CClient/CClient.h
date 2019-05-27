#ifndef CAM360_LIBCAM_CLIENT_CLIENT_H_
#define CAM360_LIBCAM_CLIENT_CLIENT_H_
#include "../PanoDef.h"
#if defined(USING_HPSOCKET)
#include "HpSocketClient.h"
#else
#include "NBSocketClient.h"
#endif
#include "../XML/XmlRpcTX2Util.h"
#include "../XML/XmlRpcTX2Value.h"
#include <memory>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
//#include <Windows.h>

#if defined(USING_HPSOCKET)
class CClient : public CHpSocketClient
#else
class CClient : public CNBSocketClient
#endif
{
private:
    std::mutex                      stream_data_callback_mtx_;
    std::function<void(LPPE_STREAM_DATA, BYTE*, int)> stream_data_callback_;
    std::mutex                      file_data_callback_mtx_;
    std::function<void(LPPE_FILE_DATA, BYTE*, int)> file_data_callback_;
    bool                            logined_;
    std::mutex                      ReceiveMsgResponseMutex;
    std::condition_variable         ReceiveMsgResponseConditionVariable;

    struct MsgResponse
    {
        char md5[33];
        XmlRpcTX2Value::XmlRpcValue result;
        std::chrono::steady_clock::time_point time;

        MsgResponse(){
            std::fill(std::begin(md5), std::end(md5), ' ');
            md5[32] = '\0';
            time = std::chrono::steady_clock::now();
        }

        MsgResponse(const MsgResponse & rhs){
            std::copy(std::begin(rhs.md5), std::end(rhs.md5), md5);
            result = rhs.result;
            time = rhs.time;
        }

        bool operator==(const MsgResponse & rhs){
            return std::string(md5) == std::string(rhs.md5);
        }

        bool operator==(std::string hash){
            return std::string(md5) == hash;
        }
    };

    std::list<MsgResponse>                msg_response_list_;
    std::mutex                            msg_response_list_mtx_;
    std::chrono::steady_clock::time_point begin_time_point_;
    std::chrono::system_clock::time_point sync_time_;

    void MakeHash(char* hash, char *input, size_t inputlen);
    bool SendEmptyMsg(size_t s);
public:
    CClient();
    virtual ~CClient();

    void SetStreamDataCallBack(void(*DataCallBack)(LPPE_STREAM_DATA lpInfo, BYTE* pData, int iLength, void *UserData), void *UserData);
    void SetFileDataCallBack(void(*DataCallBack)(LPPE_FILE_DATA lpInfo, BYTE* pData, int iLength, void *UserData), void *UserData);
    virtual void ClientAlloc() override;
    virtual void ClientRelease() override;

    bool Execute(const char* method, XmlRpcTX2Value::XmlRpcValue& params, XmlRpcTX2Value::XmlRpcValue& result, long msTime = 1000l);
    bool ProcessMsg(std::string msg);

    virtual bool Connect(const char *ipaddr, USHORT port) override;
    virtual bool Disconnect() override;

    bool Login(const char* username, const char* password);
    bool Logout();
//    int AddNewCam(const char* ipaddr, const char* dev_name, const char *serial_no);
//    int DeleteOneCam(const char* uuid);

    typedef struct tag_RemotePanoSetup{
        char	ip[64];
        char    uuid[64];
        char	devName[256];
        char	serial_no[64];
        uchar	capture_type;
        uchar	ptz_type;
        int		model_type;
        int		sub_model_type;
    }RemotePanoSetup, *LPRemotePanoSetup;
    bool GetCamList(RemotePanoSetup **ps, size_t *num);
    bool FreeCamList(RemotePanoSetup *ps);
    int DownloadPraram(const char *serial_no, const char * filename);
//    int UpdatePraram(const char *serial_no, const char * filename);
    bool FreeSysHead(BYTE *buff);

public:
    int GetStreamPort(const char* uuid);
    bool GetStreamSysHead(const char* uuid, int stream_format, BYTE **buff, size_t *buff_size);    
//    int GetCamState();
    bool SetCamAction(const char* uuid,int action);
    bool RequestStreamData(const char* uuid, int stream_format, int *handle);
    bool SwitchStreamDataFormat(int handle, int stream_format);
    bool StopStreamData(int handle);
    struct RemoteRecordExtInfo
    {
        std::string record_time;
        std::string end_time;
        std::string full_dir;
        bool        continuity;
    };
    typedef std::vector<RemoteRecordExtInfo> RemoteRecordExtInfos;
    bool GetRecordFileList(const char* uuid, const char* begTime, const char* endTime, RemoteRecordExtInfos *info);
    bool PlayRecordFile(const char *uuid, const char *file_dir, float * total_time, struct tm *recTime, int *handle);  
    bool StopPlayRecord(int handle); 
    bool PausePlayRecord(int handle, bool bPause);
    bool GetPlayRecordState(int handle, float *cur_time);
    bool SetPlayRecordSpeed(int handle, int speed);
    bool SetPlayRecordCurrentTime(int handle, float cur_tm);
    bool SetServerConfig(const char* uuid, DWORD dwCommand, LPVOID lpInBuffer, DWORD dwInBufferSize);
    bool GetServerConfig(const char* uuid, DWORD dwCommand, LPVOID lpOutBuffer, DWORD dwOutBufferSize, LPDWORD lpBytesReturned);
    bool ControlPtzWithSpeed(const char* uuid, int cmd, int ptz_speed);
    bool GetFileByName(const char *file_dir, DWORD dwOffset, LPDWORD dwFileSize, int *handle);
    bool StopGetFile(int handle);

    std::chrono::system_clock::time_point GetSyncTimePoint();
private:
    virtual bool OnClientReceive(PanoComm pc, BYTE *msg_body, size_t msg_body_size) override;
    virtual bool OnClientClose() override;
};

#endif

//#define  LOG    "JavaCallCDemoLog" // 这个是自定义的LOG的标识
//#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__) // 定义LOGD类型
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__) // 定义LOGI类型
//#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG,__VA_ARGS__) // 定义LOGW类型
//#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG,__VA_ARGS__) // 定义LOGE类型
//#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG,__VA_ARGS__) // 定义LOGF类型