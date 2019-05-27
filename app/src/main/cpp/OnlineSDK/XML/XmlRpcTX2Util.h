#ifndef CAM360_LIB_XMLRPCTX2UTIL_XMLRPCTX2UTIL_H_
#define CAM360_LIB_XMLRPCTX2UTIL_XMLRPCTX2UTIL_H_
#include <time.h>
#include <map>
#include <memory>
#include <exception>
#include <string>
#include <vector>
#include "tinyxml2.h"
#include "../../timeb.h"

namespace XmlRpcTX2Util{
    //for server
    std::string generateResponse();
    bool generateResponse(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** params);
    std::string generateResponse(int fault_code, const char *fault_string);
    std::string generateRespnseHeader(const char * server, timeb *time, const char * type, int length);
    std::string generateRespnseHeader(const char * server, timeb *time, const char * type, const char * session, int length);
    bool readRequestHeader(std::string header, int *header_size, std::string *type, int *length);
    bool readRequestHeader(std::string header, int *header_size, std::string *type, std::string * session, int *length);
    bool readRequest(tinyxml2::XMLDocument *doc, const char* buff, size_t len);
    bool parseRequest(tinyxml2::XMLDocument *doc, std::string *method_name, tinyxml2::XMLElement ** params);
    bool parseRequest(tinyxml2::XMLDocument *doc, std::string *method_name, tinyxml2::XMLElement ** value, int *value_num);

    //for client
    bool generateRequest(tinyxml2::XMLDocument *doc, std::string method_name, tinyxml2::XMLElement ** params);
    std::string generateRequestHeader(const char * client, const char* host, const char * type, int length);
    std::string generateRequestHeader(const char * client, const char* host, const char * type, const char * session, int length);
    bool readResponseHeader(std::string header, int *header_size, std::string *type, int *length);
    bool readResponseHeader(std::string header, int *header_size, std::string *type, std::string * session, int *length);
    bool readResponse(tinyxml2::XMLDocument *doc, const char* buff, size_t len);
    bool parseResponse(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** params);
    bool parseResponse(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value, int *value_num);
    bool parseResponse(tinyxml2::XMLDocument *doc, int *fault_code, std::string *fault_string);

    //util
    std::string to_string(const tinyxml2::XMLDocument *doc);
    std::string to_string(timeb time);
    std::string to_string(struct tm t);
};

#endif //CAM360_LIB_XMLRPCTX2UTIL_XMLRPCTX2UTIL_H_