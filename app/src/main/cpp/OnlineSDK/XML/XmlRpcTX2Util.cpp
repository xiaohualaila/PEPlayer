#include "XmlRpcTX2Util.h"
#include <typeinfo>
#include <iostream>
#include <string>
#include "XmlRpcTX2Value.h"

using namespace tinyxml2;

namespace XmlRpcTX2Util{
    std::string generateResponse(){
        tinyxml2::XMLDocument doc;
        XMLDeclaration* decl = doc.NewDeclaration();
        doc.LinkEndChild(decl);

        XMLElement *method_response = doc.NewElement("methodResponse");
        doc.LinkEndChild(method_response);

        tinyxml2::XMLElement *params = doc.NewElement("params");
        method_response->LinkEndChild(params);

        XMLElement * param = doc.NewElement("param");
        params->LinkEndChild(param);
        XMLElement * value = doc.NewElement("value");
        param->LinkEndChild(value);

        return to_string(&doc);
    }

    bool generateResponse(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** params){
        XMLDeclaration* decl = doc->NewDeclaration();
        doc->LinkEndChild(decl);

        XMLElement *method_response = doc->NewElement("methodResponse");
        doc->LinkEndChild(method_response);

        *params = doc->NewElement("params");
        method_response->LinkEndChild(*params);

        return true;
    }

    std::string generateResponse(int fault_code, const char *fault_string){
        tinyxml2::XMLDocument doc;
        XMLDeclaration* decl = doc.NewDeclaration();
        doc.LinkEndChild(decl);

        XMLElement *method_response = doc.NewElement("methodResponse");
        doc.LinkEndChild(method_response);

        XMLElement * fault = doc.NewElement("fault");
        method_response->LinkEndChild(fault);

        XmlRpcTX2Value::XmlRpcValue fault_struct;
        fault_struct["faultCode"] = fault_code;
        fault_struct["faultString"] = fault_string;

        XMLElement * fault_value=nullptr;
        fault_struct.toXml(&doc, &fault_value);
        fault->LinkEndChild(fault_value);
        
        return to_string(&doc);
    }

    std::string generateRespnseHeader(const char * version, timeb *tbTime, const char * content_type, int content_length){
        timeb tmp;
        if (!tbTime){
            ftime(&tmp);
        }
        else{
            memcpy(&tmp, tbTime, sizeof(timeb));
        }

        std::string header;
        header += "HTTP/1.1 200 OK\r\n";
        header += "Connection: Keep-Alive\r\n";
        header += "Content-Length: " + std::to_string(content_length) + "\r\n";
        header += "Content-Type: " + std::string(content_type) + "\r\n";
        header += "Date: " + to_string(tmp) + "\r\n";
        header += "Server: " + std::string(version) + "\r\n\r\n";

        return header;
    }

    std::string generateRespnseHeader(const char * version, timeb *tbTime, const char * content_type, const char * hash, int content_length){
        timeb tmp;
        if (!tbTime){
            ftime(&tmp);
        }
        else{
            memcpy(&tmp, tbTime, sizeof(timeb));
        }

        std::string header;
        header += "HTTP/1.1 200 OK\r\n";
        header += "Connection: Keep-Alive\r\n";
        header += "Content-Length: " + std::to_string(content_length) + "\r\n";
        header += "Content-Type: " + std::string(content_type) + "\r\n";
        header += "Date: " + to_string(tmp) + "\r\n";
        header += "Server: " + std::string(version) + "\r\n";
        header += "Cookie: " + std::string(hash) + "\r\n\r\n";

        return header;
    }


    bool readRequest(tinyxml2::XMLDocument *doc, const char* buff, size_t len){
        return XML_NO_ERROR == doc->Parse(buff, len);
    }

    bool parseRequest(tinyxml2::XMLDocument *doc, std::string *method_name, tinyxml2::XMLElement ** params){
        try{
            XMLElement * methodCall = doc->FirstChildElement("methodCall");
            typeid(*methodCall);
            XMLElement * methodName = methodCall->FirstChildElement("methodName");
            typeid(*methodName);
            *method_name = methodName->FirstChild()->Value();
            *params = methodName->NextSiblingElement("params");
            typeid(**params);
            return true;
        }
        catch (std::bad_typeid){
            return false;
        }
        return false;
    }

    bool parseRequest(tinyxml2::XMLDocument *doc, std::string *method_name, tinyxml2::XMLElement ** value, int *value_num){
        try{
            XMLElement * methodCall = doc->FirstChildElement("methodCall");
            typeid(*methodCall);
            XMLElement * methodName = methodCall->FirstChildElement("methodName");
            typeid(*methodName);
            *method_name = methodName->FirstChild()->Value();
            tinyxml2::XMLElement *params = methodName->NextSiblingElement("params");
            typeid(*params);
            XMLElement *param = params->FirstChildElement("param");
            std::vector<XMLElement *> value_vector;
            while (param){
                XMLElement *value_iter = param->FirstChildElement("value");
                if (value_iter)
                    value_vector.push_back(value_iter);
                param = param->NextSiblingElement("param");
            }

            if (!value_num){
                return false;
            }
            else if (*value_num < 0){
                *value_num = value_vector.size();
                return true;
            }
            else if (*value_num > (int)value_vector.size()){
                *value_num = value_vector.size();
            }

            if (value){
                for (int i = 0; i < (int)value_vector.size() && i < *value_num; ++i)
				{
                    value[i] = value_vector.at(i);
                }
            }

            return true;
        }
        catch (std::bad_typeid)
		{
            return false;
        }
        return false;
    }

    bool readRequestHeader(std::string header, int *header_size, std::string *type, int *length){
        bool ret = false;

        auto lp = header.find("Content-Length: ");    // Start of content-length value
        auto tp = header.find("Content-Type: ");      // Start of content-type value
        auto bp = header.rfind("\r\n\r\n");           // Start of body

        if (lp == header.npos || tp == header.npos || bp == header.npos){
            return false;
        }

        auto lep = header.find("\r\n", lp);
        std::string Content_LengthValueStr(header, lp + 16, lep - (lp + 16));

        auto tep = header.find("\r\n", tp);
        std::string Content_TypeValueStr(header, tp + 14, tep - (tp + 14));

        *header_size = bp + 4;

        if (Content_TypeValueStr.size() > 0){
            if ("binary/h264" == Content_TypeValueStr){
                *type = "H264";
                ret = true;
            }
            else if ("text/xml" == Content_TypeValueStr){
                *type = "xml";
                ret = true;
            }
        }

        *length = std::stoi(Content_LengthValueStr);
        if (*length > 0){
            ret &= true;
        }
        else{
            ret &= false;
        }

        return ret;
    }

    bool readRequestHeader(std::string header, int *header_size, std::string *type, std::string * session, int *length){
        bool ret = false;

        auto lp = header.find("Content-Length: ");    // Start of content-length value
        auto tp = header.find("Content-Type: ");      // Start of content-type value
        auto cp = header.find("Cookie: ");           // Start of content-type value
        auto bp = header.rfind("\r\n\r\n");           // Start of body

        if (lp == header.npos || tp == header.npos || cp == header.npos || bp == header.npos){
            return false;
        }

        auto lep = header.find("\r\n", lp);
        std::string Content_LengthValueStr(header, lp + 16, lep - (lp + 16));

        auto tep = header.find("\r\n", tp);
        std::string Content_TypeValueStr(header, tp + 14, tep - (tp + 14));

        auto cep = header.find("\r\n", cp);
        *session = std::string(header, cp + 8, cep - (cp + 8));
        
        *header_size = bp + 4;

        if (Content_TypeValueStr.size() > 0){
            if ("binary/h264" == Content_TypeValueStr){
                *type = "H264";
                ret = true;
            }
            else if ("text/xml" == Content_TypeValueStr){
                *type = "xml";
                ret = true;
            }
        }

        *length = std::stoi(Content_LengthValueStr);
        if (*length > 0){
            ret &= true;
        }
        else{
            ret &= false;
        }

        return ret;
    }

    bool generateRequest(tinyxml2::XMLDocument *doc, std::string method_name, tinyxml2::XMLElement ** params){
        XMLDeclaration* decl = doc->NewDeclaration();
        doc->LinkEndChild(decl);

        XMLElement * methodCall = doc->NewElement("methodCall");
        doc->LinkEndChild(methodCall);

        XMLElement * methodName = doc->NewElement("methodName");
        methodName->LinkEndChild(doc->NewText(method_name.c_str()));
        methodCall->LinkEndChild(methodName);

        *params = doc->NewElement("params");
        methodCall->LinkEndChild(*params);

        return true;
    }

    std::string generateRequestHeader(const char * client, const char* host, const char * type, int length){
        std::string header;
        header += "POST /RPC2 HTTP/1.1\r\n";
        header += "User-Agent: " + std::string(client) + "\r\n";
        header += "Host: " + std::string(host) + "\r\n";
        header += "Content-Type: " + std::string(type) + "\r\n";
        header += "Content-Length: " + std::to_string(length) + "\r\n\r\n";

        return header;
    }

    std::string generateRequestHeader(const char * client, const char* host, const char * type, const char * hash, int length){
        std::string header;
        header += "POST /RPC2 HTTP/1.1\r\n";
        header += "User-Agent: " + std::string(client) + "\r\n";
        header += "Host: " + std::string(host) + "\r\n";
        header += "Content-Type: " + std::string(type) + "\r\n";
        header += "Content-Length: " + std::to_string(length) + "\r\n";
        header += "Cookie: " + std::string(hash) + "\r\n\r\n";

        return header;
    }

    bool readResponseHeader(std::string header, int *header_size, std::string *type, int *length){
        bool ret = false;

        auto lp = header.find("Content-Length: ");    // Start of content-length value
        auto tp = header.find("Content-Type: ");      // Start of content-type value
        auto bp = header.rfind("\r\n\r\n");           // Start of body

        if (lp == header.npos || tp == header.npos || bp == header.npos){
            return false;
        }

        auto lep = header.find("\r\n", lp);
        std::string Content_LengthValueStr(header, lp + 16, lep - (lp + 16));

        auto tep = header.find("\r\n", tp);
        std::string Content_TypeValueStr(header, tp + 14, tep - (tp + 14));

        *header_size = bp + 4;

        if (Content_TypeValueStr.size() > 0){
            ret = true;
            if ("binary/h264" == Content_TypeValueStr){
                *type = "H264";
            }
            else if ("text/xml" == Content_TypeValueStr){
                *type = "xml";
            }
            else{
                auto lp = Content_TypeValueStr.find("/");
                if (lp != Content_TypeValueStr.npos){
                    *type = std::string(Content_TypeValueStr, lp+1, Content_TypeValueStr.npos);
                }
                else{
                    *type = Content_TypeValueStr;
                }
            }
        }
        else{
            ret = false;
        }

        *length = std::stoi(Content_LengthValueStr);
        if (*length > 0){
            ret &= true;
        }
        else{
            ret &= false;
        }

        return ret;
    }

    bool readResponseHeader(std::string header, int *header_size, std::string *type, std::string * session, int *length){
        bool ret = false;

        auto lp = header.find("Content-Length: ");    // Start of content-length value
        auto tp = header.find("Content-Type: ");      // Start of content-type value
        auto cp = header.find("Cookie: ");           // Start of content-type value
        auto bp = header.rfind("\r\n\r\n");           // Start of body

        if (lp == header.npos || tp == header.npos || cp == header.npos || bp == header.npos){
            return false;
        }

        auto lep = header.find("\r\n", lp);
        std::string Content_LengthValueStr(header, lp + 16, lep - (lp + 16));

        auto tep = header.find("\r\n", tp);
        std::string Content_TypeValueStr(header, tp + 14, tep - (tp + 14));

        auto cep = header.find("\r\n", cp);
        *session = std::string(header, cp + 8, cep - (cp + 8));

        *header_size = bp + 4;

        if (Content_TypeValueStr.size() > 0){
            ret = true;
            if ("binary/h264" == Content_TypeValueStr){
                *type = "H264";
            }
            else if ("text/xml" == Content_TypeValueStr){
                *type = "xml";
            }
            else{
                auto lp = Content_TypeValueStr.find("/");
                if (lp != Content_TypeValueStr.npos){
                    *type = std::string(Content_TypeValueStr, lp + 1, Content_TypeValueStr.npos);
                }
                else{
                    *type = Content_TypeValueStr;
                }
            }
        }
        else{
            ret = false;
        }

        *length = std::stoi(Content_LengthValueStr);
        if (*length > 0){
            ret &= true;
        }
        else{
            ret &= false;
        }

        return ret;
    }

    bool readResponse(tinyxml2::XMLDocument *doc, const char* buff, size_t len){
        return XML_NO_ERROR == doc->Parse(buff, len);
    }

    bool parseResponse(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** params){
        try{
            XMLElement * methodResponse = doc->FirstChildElement("methodResponse");
            typeid(*methodResponse);
            *params = methodResponse->FirstChildElement("params");
            typeid(**params);

            return true;
        }
        catch (std::bad_typeid){
            return false;
        }
        return false;
    }

    bool parseResponse(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value, int *value_num){
        try{
            XMLElement * methodResponse = doc->FirstChildElement("methodResponse");
            typeid(*methodResponse);
            XMLElement *params = methodResponse->FirstChildElement("params");
            typeid(*params);
            XMLElement *param = params->FirstChildElement("param");
            std::vector<XMLElement *> value_vector;
            while (param){
                XMLElement *value_iter = param->FirstChildElement("value");
                if (value_iter)
                    value_vector.push_back(value_iter);
                param = param->NextSiblingElement("param");
            }

            if (!value_num){
                return false;
            }
            else if (*value_num <= 0){
                *value_num = value_vector.size();
                return true;
            }
            else if (*value_num > (int) value_vector.size()){
                *value_num = value_vector.size();
            }

            if (value){
                for (int i = 0; i < (int) value_vector.size() && i < *value_num; ++i){
                    value[i] = value_vector.at(i);
                }
            }

            return true;
        }
        catch (std::bad_typeid){
            return false;
        }
        return false;
    }

    bool parseResponse(tinyxml2::XMLDocument *doc, int *fault_code, std::string *fault_string){
        try{
            XMLElement * methodResponse = doc->FirstChildElement("methodResponse");
            typeid(*methodResponse);
            XMLElement * fault = methodResponse->FirstChildElement("fault");
            typeid(*fault);
            XMLElement * fault_value = fault->FirstChildElement("value");
            typeid(*fault_value);
            XmlRpcTX2Value::XmlRpcValue fault_struct(fault_value);
            *fault_code = fault_struct["faultCode"];
            *fault_string = std::string(fault_struct["faultString"]);
            return true;
        }
        catch (std::bad_typeid){
            return false;
        }
        catch (std::exception){
            return false;
        }
        return false;
    }

    std::string to_string(const tinyxml2::XMLDocument *doc){
        XMLPrinter printer;
        doc->Accept(&printer);
        return std::string(printer.CStr());
    }

    std::string to_string(timeb tbTime){
        struct tm *p;
        p=gmtime(&tbTime.time);
        char buf[512];
        strftime(buf, 512, "%a, %d %b %Y %X GMT", p);
        return std::string(buf);
    }

    std::string to_string(struct tm t){
        char buf[20];
        sprintf(buf, "%4d%02d%02dT%02d:%02d:%02d",
            t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
        buf[sizeof(buf)-1] = 0;
        return std::string(buf);
    }
}
