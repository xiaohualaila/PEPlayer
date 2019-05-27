#include "CClient.h"
#include "../PanoDef.h"
//#include "../ZipLibUtil/ZipLibUtil.h"
extern "C" {
#include "../md5/md5.cpp"
}
#include <iostream>
//#include <Windows.h>

CClient::CClient()
{
	logined_ = false;
	begin_time_point_ = std::chrono::steady_clock::now();
}

CClient::~CClient()
{
	Logout();
}

void CClient::ClientAlloc()
{
	try
	{
#if defined(USING_HPSOCKET)
        CHpSocketClient::ClientAlloc();
#else
        CNBSocketClient::ClientAlloc();
#endif
	}
    catch (std::bad_alloc){
        //HandleException(__FILE__, __LINE__);
    }
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
}

void CClient::ClientRelease()
{
	try
	{
#if defined(USING_HPSOCKET)
        CHpSocketClient::ClientRelease();
#else
        CNBSocketClient::ClientRelease();
#endif
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
}

void CClient::SetStreamDataCallBack(void(*DataCallBack)(LPPE_STREAM_DATA lpInfo, BYTE* pData, int iLength, void *UserData), void *UserData)
{
    try
    {
        std::lock_guard<std::mutex> lck(stream_data_callback_mtx_);
        stream_data_callback_ = std::bind(DataCallBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, UserData);
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
}

void CClient::SetFileDataCallBack(void(*DataCallBack)(LPPE_FILE_DATA lpInfo, BYTE* pData, int iLength, void *UserData), void *UserData)
{
    try
    {
        std::lock_guard<std::mutex> lck(file_data_callback_mtx_);
        file_data_callback_ = std::bind(DataCallBack, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, UserData);
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
}

void CClient::MakeHash(char* hash, char *input, size_t inputlen)
{
	try
	{
		auto now_time_point = std::chrono::steady_clock::now();
		auto t = std::chrono::duration_cast<std::chrono::microseconds>(now_time_point - begin_time_point_);
		auto count = t.count();

		unsigned char decrypt[16];
		MD5_CTX md5;
		MD5Init(&md5);
		MD5Update(&md5, (unsigned char*)input, inputlen);
		MD5Update(&md5, (unsigned char*)&count, sizeof(count));
		MD5Final(&md5, decrypt);
		for (int i = 0; i < 16; i++)
		{
			sprintf(&hash[2 * i], "%02x", decrypt[i]);
		}
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
}

bool CClient::SendEmptyMsg(size_t s)
{
#if defined(USING_HPSOCKET)
	WSABUF buffers[2];
	try
	{
		char buff[4096];
		PanoComm pc;
		std::fill(std::begin(buff), std::end(buff), 0);
        PanoComm_Init(&pc);
		pc.command = CT_EMPTY;
        pc.pkgSize = PanoComm_Size() + s;
		buffers[0].buf = (CHAR*)&pc;
		buffers[0].len = PanoComm_Size();
		buffers[1].buf = (CHAR*)buff;
		buffers[1].len = s;
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}

	return SendPackets(buffers, s == 0 ? 1 : 2);
#else
    return true;
#endif
}

bool CClient::Execute(const char* method, XmlRpcTX2Value::XmlRpcValue& val,
	XmlRpcTX2Value::XmlRpcValue& result, long msTime /*= 1000l*/)
{
	try
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement * params;

		XmlRpcTX2Util::generateRequest(&doc, method, &params);
		tinyxml2::XMLElement * param = doc.NewElement("param");

		tinyxml2::XMLElement * value = nullptr;
		if (val.toXml(&doc, &value))
		{
			param->LinkEndChild(value);
			params->LinkEndChild(param);
		}

		std::string xml = XmlRpcTX2Util::to_string(&doc);

		char hash[33];
		MakeHash(hash, (char*)xml.c_str(), xml.length());

		std::string header = XmlRpcTX2Util::generateRequestHeader("CClient1.0", GetHost().c_str(), "text/xml", hash, xml.length());

		WSABUF buffers[3];
		PanoComm pc;
        PanoComm_Init(&pc);
		pc.command = CT_XMLRPC_OVER_PES_REQUEST;
		pc.pkgSize = PanoComm_Size() + header.length() + xml.length();
		buffers[0].buf = (CHAR*)&pc;
		buffers[0].len = PanoComm_Size();
		buffers[1].buf = (CHAR*)header.data();
		buffers[1].len = header.length();
		buffers[2].buf = (CHAR*)xml.data();
		buffers[2].len = xml.length();

		if (pc.pkgSize < 4096)
		{
			SendEmptyMsg(4096 - pc.pkgSize);
		}

		if (SendPackets(buffers, 3))
		{
			msTime = 5000;
            auto end_time_point = std::chrono::steady_clock::now() + \
                std::chrono::milliseconds(msTime < 0 ? 60 * 1000 : msTime);
			while (true)
			{
                {
                    std::unique_lock<std::mutex>    lck(ReceiveMsgResponseMutex);
                    if (std::cv_status::timeout == ReceiveMsgResponseConditionVariable.wait_until(lck, end_time_point))
                        return false;
                }

				std::lock_guard<std::mutex> lck(msg_response_list_mtx_);
				for (auto msg_response = std::begin(msg_response_list_); msg_response != std::end(msg_response_list_);){
					if (*msg_response == std::string(hash))
					{
						result = msg_response->result;
						msg_response = msg_response_list_.erase(msg_response);
                        return true;
					}
					else
					{
						++msg_response;
					}
				}
			}
			return true;
		}
	}
	catch (...)//bad_alloc)
	{
		//HandleException(__FILE__, __LINE__);
	}
	return false;
}

bool CClient::ProcessMsg(std::string msg)
{
	try
	{
		int header_size = 0;
		std::string strType;
		std::string hash;
		int data_size = -1;
		if (XmlRpcTX2Util::readResponseHeader(msg, &header_size, &strType, &hash, &data_size))
		{
			std::string msg_body;
			std::copy(std::begin(msg), std::end(msg), std::back_inserter(msg_body));

			tinyxml2::XMLDocument doc;
			tinyxml2::XMLElement * params = nullptr;
			XmlRpcTX2Util::readResponse(&doc, msg_body.c_str(), msg_body.length());

			int value_num = 0;
			if (XmlRpcTX2Util::parseResponse(&doc, nullptr, &value_num))
			{
				XmlRpcTX2Value::XmlRpcValue result;
				if (value_num > 0)
				{
					tinyxml2::XMLElement ** value = new tinyxml2::XMLElement *[value_num];
					if (XmlRpcTX2Util::parseResponse(&doc, value, &value_num))
					{
						if (value_num > 1)
						{
							for (int id = 0; id < value_num; ++id)
							{
								result[id] = XmlRpcTX2Value::XmlRpcValue(value[id]);
							}
						}
						else if (value_num == 1)
						{
							result = XmlRpcTX2Value::XmlRpcValue(value[0]);
						}

					}
					delete[] value;
				}

				std::unique_lock<std::mutex> lck(msg_response_list_mtx_);
				MsgResponse msg_response;
				std::copy(std::begin(hash), std::end(hash), std::begin(msg_response.md5));
				msg_response.result = result;
				msg_response_list_.push_back(msg_response);
				lck.unlock();

                ReceiveMsgResponseConditionVariable.notify_all();

				lck.lock();
				auto time_point = std::chrono::steady_clock::now();
				for (auto msg_response = std::begin(msg_response_list_); msg_response != std::end(msg_response_list_);)
				{
					auto dt = std::chrono::duration_cast<std::chrono::minutes>(time_point - msg_response->time);
					if (dt > std::chrono::minutes(2))
					{
						msg_response = msg_response_list_.erase(msg_response);
					}
					else
					{
						++msg_response;
					}
				}
				lck.unlock();
				return true;
			}
		}
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
	return false;
}

bool CClient::Connect(const char *ipaddr, USHORT port)
{
	try
	{
#if defined(USING_HPSOCKET)
        if (CHpSocketClient::Connect(ipaddr, port))
		{
			SendEmptyMsg(4096);
			SendEmptyMsg(4096);
			SendEmptyMsg(4096);
			SendEmptyMsg(4096);
		}
		else
		{
			return false;
		}	
#else
        return CNBSocketClient::Connect(ipaddr, port);
#endif
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
	return true;
}

bool CClient::Disconnect()
{
	try
	{
#if defined(USING_HPSOCKET)
        return CHpSocketClient::Disconnect();
#else
        return CNBSocketClient::Disconnect();
#endif
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
	return false;
}
#define LOGIN_SUCCESS 0
#define LOGIN_FAILED_TIMEOUT 1
#define LOGIN_FAILED_USERERROR 2
bool CClient::Login(const char* username, const char* password)
{
	try
	{
		XmlRpcTX2Value::XmlRpcValue params, result;
		params[0] = username;
		params[1] = password;
		if (Execute("CT_LOGIN", params, result))
		{
			if (bool(result[0]))
			{
				//const XmlRpcTX2Value::XmlRpcValue::BinaryData& data = result[1];
				//memcpy(&RemoteConnID, data.data(), sizeof(RemoteConnID));
				logined_ = true;
				return LOGIN_SUCCESS;
			} else {
				return LOGIN_FAILED_USERERROR;
			}
		} else {
			return LOGIN_FAILED_TIMEOUT;
		}
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
	return false;
}

bool CClient::Logout()
{
	logined_ = false;
	return true;
}

bool CClient::GetCamList(RemotePanoSetup **lps, size_t *num)
{
    if (!logined_)
        return false;

	try
	{
		XmlRpcTX2Value::XmlRpcValue params, result;
		if (Execute("CT_CAM_LIST_V2", params, result))
		{
			if (bool(result[0]))
			{
				if (result.size() > 1)
				{
					*num = result[1].size();
					if (*num > 0){
						RemotePanoSetup *ps = new RemotePanoSetup[*num];
						for (size_t id = 0; id < *num; ++id)
						{
							strcpy(ps[id].ip, std::string(result[1][id]["ip"]).c_str());
							strcpy(ps[id].serial_no, std::string(result[1][id]["serial_no"]).c_str());
							strcpy(ps[id].devName, std::string(result[1][id]["devName"]).c_str());
							strcpy(ps[id].uuid, std::string(result[1][id]["uuid"]).c_str());							ps[id].capture_type = int(result[1][id]["capture_type"]);
							ps[id].model_type = int(result[1][id]["model_type"]);
							ps[id].sub_model_type = int(result[1][id]["sub_model_type"]);
							ps[id].ptz_type = int(result[1][id]["ptz_type"]);
						}
						*lps = ps;
					}
					else
					{
						*lps = nullptr;
					}
				}
				return true;
			}
		}
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}
	return false;
}

bool CClient::FreeCamList(RemotePanoSetup *ps)
{
	delete[] ps;
	return true;
}

int CClient::DownloadPraram(const char *serial_no, const char * filename)
{
	try
	{
		XmlRpcTX2Value::XmlRpcValue params, result;
		params[0] = serial_no;
		if (Execute("CT_DOWNLOAD_PRARAM", params, result, -1))
		{
			if (bool(result[0])){
				const XmlRpcTX2Value::XmlRpcValue::BinaryData& data = result[1];
				FILE * fp = nullptr;
				fp = fopen(filename, "wb+");
				if (0 == fp)
				{
					//ExtracMemory2File((unsigned char*)&data[0], data.size(), fp);
					fclose(fp);
					return true;
				}
			}
		}
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}

	return false;
}

int CClient::GetStreamPort(const char* uuid)
{
	if (!logined_)
		return -1;

	try
	{
		XmlRpcTX2Value::XmlRpcValue params, result;
		params[0] = uuid;
		//printf("CClient::GetPort()................\n");

		if (Execute("CT_REQUEST_PORT_BY_UUID", params, result))
		{
			return int(result[1]);
		}
	}
	catch (...)
	{
		//ClientHandleException(__FILE__, __LINE__);
	}
	return -1;
}

bool CClient::GetStreamSysHead(const char* uuid, int stream_format, BYTE **buff, size_t *buff_size)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = uuid;
        params[1] = stream_format;
        if (Execute("CT_GET_STREAM_SYS_HEAD_BY_UUID", params, result))
        {
            if (bool(result[0]))
            {
                XmlRpcTX2Value::XmlRpcValue::BinaryData& data = result[1];
                *buff_size = data.size();
                *buff = new BYTE[*buff_size];
                std::copy(std::begin(data), std::end(data), *buff);
                return true;
            }
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }

    return false;
}

bool CClient::FreeSysHead(BYTE *buff)
{
	delete[] buff;
	return true;
}

bool CClient::SetCamAction(const char* uuid, int action)
{
	if (!logined_)
		return false;

	try
	{
		XmlRpcTX2Value::XmlRpcValue params, result;
		params[0] = std::string(uuid);
		params[1] = action;
		if (Execute("CT_SET_CAM_ACTION", params, result))
		{
			return bool(result[0]);
		}
	}
	catch (...)
	{
		//HandleException(__FILE__, __LINE__);
	}

	return false;
}

bool CClient::RequestStreamData(const char* uuid, int stream_format, int *handle)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = uuid;
        params[1] = stream_format;
        if (Execute("CT_REQUEST_REALPLAY_BY_UUID", params, result))
        {
            if (bool(result[0])){
                *handle = result[1];
                return true;
            }
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }

    return false;
}

bool CClient::SwitchStreamDataFormat(int handle, int stream_format){
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = handle;
        params[1] = stream_format;
        if (Execute("CT_SWITCH_STREAM_FORMAT", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }

    return false;
}

bool CClient::StopStreamData(int handle)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = handle;
        if (Execute("CT_STOP_REALPLAY_BY_HANDLE", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }

    return false;
}

bool CClient::GetRecordFileList(const char* uuid, const char* begTime, const char* endTime, RemoteRecordExtInfos *info){
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = uuid;
        params[1] = begTime;
        params[2] = endTime;
        if (Execute("CT_GET_RECORD_FILE_LIST_BY_UUID", params, result))
        {
            if (bool(result[0]))
            {
                if (result.size() > 1)
                {
                    for (int id = 0; id < result[1].size(); ++id)
                    {
                        std::string record_time = result[1][id][0];
                        std::string full_dir = result[1][id][1];
                        std::string end_time = result[1][id][2];
                        bool        continuity = result[1][id][3];
                        info->push_back({ record_time, end_time, full_dir, continuity });
                    }
                }
                return true;
            }
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::PlayRecordFile(const char *uuid, const char *file_dir, float *total_time, struct tm *recTime, int *handle)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = uuid;
        params[1] = file_dir;
        if (Execute("CT_PLAY_RECORD_FILE_BY_UUID", params, result))
        {
            if (bool(result[0]))
            {
                *total_time = (float)double(result[1]);
                XmlRpcTX2Value::XmlRpcValue::BinaryData& data = result[2];
                std::copy(std::begin(data), std::end(data), (char*)recTime);
                *handle = int(result[3]);
                return true;
            }
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::StopPlayRecord(int handle)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = handle;
        if (Execute("CT_STOP_PLAY_RECORD_BY_HANDLE", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::PausePlayRecord(int handle, bool bPause)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = handle;
        params[1] = bPause;
        if (Execute("CT_PAUSE_PLAY_RECORD_BY_HANDLE", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::GetPlayRecordState(int handle, float *cur_time)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = handle;
        if (Execute("CT_GET_PLAY_RECORD_STATE_BY_HANDLE", params, result))
        {
            if (bool(result[0]))
            {
                *cur_time = (float)double(result[1]);
                return bool(result[2]);
            }
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::SetPlayRecordSpeed(int handle, int speed)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = handle;
        params[1] = speed;
        if (Execute("CT_SET_PLAY_RECORD_SPEED_BY_HANDLE", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::SetPlayRecordCurrentTime(int handle, float cur_tm)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = handle;
        params[1] = (float)double(cur_tm);
        if (Execute("CT_SET_PLAY_RECORD_CURRENT_TIME_BY_HANDLE", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::ControlPtzWithSpeed(const char* uuid, int cmd, int ptz_speed)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = uuid;
        params[1] = cmd;
        params[2] = ptz_speed;
        if (Execute("CT_CONTROL_PTZ_WITH_SPEED_BY_UUID", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::SetServerConfig(const char* uuid, DWORD dwCommand, LPVOID lpInBuffer, DWORD dwInBufferSize)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = std::string(uuid);
        params[1] = int(dwCommand);

        if (dwCommand == CT_SET_ZERO_CHAN_CFG)
        {
            
        }
        else if (dwCommand == CT_SET_CONNECT_CFG)
        {
            int *action = (int*)lpInBuffer;
            params[2] = *action;
        }
        else if (dwCommand == CT_SET_PTZ_POS)
        {
            LPPE_PTZ_POS_CFG cfg = (LPPE_PTZ_POS_CFG)lpInBuffer;
            params[2] = double(cfg->fTiltPos);
            params[3] = double(cfg->fPanPos);
            params[4] = int(cfg->fZoomPos);
        }
        else
        {
            return false;
        }

        if (Execute("CT_SET_SERVER_CONFIG", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::GetServerConfig(const char* uuid, DWORD dwCommand, LPVOID lpOutBuffer, DWORD dwOutBufferSize, LPDWORD lpBytesReturned)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = std::string(uuid);
        params[1] = int(dwCommand);
        if (Execute("CT_GET_SERVER_CONFIG", params, result, 10000))
        {
            if (bool(result[0])){
                if (dwCommand == CT_GET_CONNECT_CFG)
                {
                    LPPE_CONNECT_CFG cfg = (LPPE_CONNECT_CFG)lpOutBuffer;
                    *lpBytesReturned = sizeof(PE_CONNECT_CFG);

                    cfg->byNetState = int(result[1]);
                    cfg->byStopConnect = int(result[2]);
                    cfg->byAutoConnect = int(result[3]);
                    cfg->bySubStreamOnly = int(result[4]);
                    return true;
                }
                else if (dwCommand == CT_GET_PTZ_POS)
                {
                    LPPE_PTZ_POS_CFG cfg = (LPPE_PTZ_POS_CFG)lpOutBuffer;
                    *lpBytesReturned = sizeof(PE_PTZ_POS_CFG);
                    cfg->fTiltPos = double(result[1]);
                    cfg->fPanPos = double(result[2]);
                    cfg->fZoomPos = int(result[3]);
                    return true;
                }
            }
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::GetFileByName(const char *file_dir, DWORD dwOffset, LPDWORD dwFileSize, int *handle)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = std::string(file_dir);
        params[1] = int(dwOffset);
        if (Execute("CT_GET_FILE_BY_NAME", params, result))
        {
            if (bool(result[0])){
                *handle = int(result[1]);
                *dwFileSize = int(result[2]);
                return true;
            }
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

bool CClient::StopGetFile(int handle)
{
    if (!logined_)
        return false;

    try
    {
        XmlRpcTX2Value::XmlRpcValue params, result;
        params[0] = int(handle);
        if (Execute("CT_STOP_GET_FILE_BY_HANDLE", params, result))
        {
            return bool(result[0]);
        }
    }
    catch (...)
    {
        //HandleException(__FILE__, __LINE__);
    }
    return false;
}

std::chrono::system_clock::time_point CClient::GetSyncTimePoint(){
    return sync_time_;
}

bool CClient::OnClientReceive(PanoComm pc, BYTE *msg_body, size_t msg_body_size)
{
    int   msg_type = pc.command;
    switch (msg_type)
    {
    case CT_STREAM_ZERO_CHAN_DATA:
        break;
    case CT_XMLRPC_OVER_PES_RESPONSE:
    {
        std::string msg;
        std::copy_n(msg_body, msg_body_size, std::back_inserter(msg));
        std::thread(std::bind(&CClient::ProcessMsg, this, msg)).detach();
    }
    break;
    case CT_SERVER_VERSION_SEND:
        break;
    case CT_STREAM_ZERO_CHAN_TIME_SYNC:
    {
        int data_size = sizeof(PE_StreamZeroChanData);
        PE_StreamZeroChanData *sd = (PE_StreamZeroChanData *)(msg_body);
        uchar *raw_data = msg_body + data_size;
        int raw_data_length = msg_body_size - data_size;
        long long dt;
        memcpy(&dt, raw_data, sizeof(dt));
        sync_time_ = std::chrono::system_clock::time_point(std::chrono::milliseconds(dt));
    }
    break;
    case CT_STREAM_DATA:
    {
        PE_STREAM_DATA struStreamInfo;
        memset(&struStreamInfo, 0, sizeof(struStreamInfo));
        struStreamInfo.dwSize = sizeof(struStreamInfo);

        LPPE_STREAM_DATA lpInfo = (LPPE_STREAM_DATA)(msg_body);
        uchar *raw_data = msg_body + lpInfo->dwSize;
        int raw_data_length = msg_body_size - lpInfo->dwSize;

		DWORD dwSize = std::min(struStreamInfo.dwSize, lpInfo->dwSize);
        memcpy(&struStreamInfo, lpInfo, dwSize);

        std::lock_guard<std::mutex> lck(stream_data_callback_mtx_);
        if (stream_data_callback_)
            stream_data_callback_(&struStreamInfo, raw_data, raw_data_length);
    }
    break;
    case CT_FILE_TRANSLATE:
    {
        PE_FILE_DATA struFileInfo;
        memset(&struFileInfo, 0, sizeof(struFileInfo));
        struFileInfo.dwSize = sizeof(struFileInfo);

        LPPE_FILE_DATA lpInfo = (LPPE_FILE_DATA)(msg_body);
        uchar *raw_data = msg_body + lpInfo->dwSize;
        int raw_data_length = msg_body_size - lpInfo->dwSize;

        DWORD dwSize = std::min(struFileInfo.dwSize, lpInfo->dwSize);
        memcpy(&struFileInfo, lpInfo, dwSize);

        std::lock_guard<std::mutex> lck(file_data_callback_mtx_);
        if (file_data_callback_)
            file_data_callback_(&struFileInfo, raw_data, raw_data_length);
    }
    break;
    default:
        break;
    }
    return true;
}

bool CClient::OnClientClose()
{
    Logout();
    return true;
}
