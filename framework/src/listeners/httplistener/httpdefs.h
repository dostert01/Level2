#pragma once


#define HTTP_STATUS_CODE_200 "200 OK"
#define HTTP_STATUS_CODE_400 "400 Bad Request"
#define HTTP_STATUS_CODE_401 "401 Unauthorized"
#define HTTP_STATUS_CODE_403 "403 Forbidden"
#define HTTP_STATUS_CODE_404 "404 Not Found"
#define HTTP_STATUS_CODE_408 "408 Request Timeout"
#define HTTP_STATUS_CODE_411 "411 Length Required"
#define HTTP_STATUS_CODE_413 "413 Payload Too Large"
#define HTTP_STATUS_CODE_431 "431 Request Header Fields Too Large"
#define HTTP_STATUS_CODE_500 "500 Internal Server Error"
#define HTTP_STATUS_CODE_505 "505 HTTP Version Not Supported"

#define HTTP_PROTOCOL_VERSION "HTTP/1.1"

#define HTTP_FIELD_NAME_CONNECTION "Connection"
#define HTTP_FIELD_NAME_CONTENT_TYPE "Content-Type"
#define HTTP_FIELD_NAME_CONTENT_LENGTH "Content-Length"
#define HTTP_FIELD_NAME_SERVER "Server"

#define HTTP_FIELD_VALUE_CLOSE "close"
#define HTTP_FIELD_VALUE_SERVER "level2 httpListener"

#define READ_BUFFER_SIZE 2049
#define MAX_READ (READ_BUFFER_SIZE - 1)
#define LINE_TERMINATOR "\r\n"
#define HEADER_TERMINATOR std::string("\r\n\r\n").c_str()
#define HEADER_FIELD_SEPARATOR ": "
#define METHOD_PATH_SEPARATOR " /"
#define PATH_PROTOCOL_SEPARATOR " " HTTP_PROTOCOL_VERSION
#define PATH_QUERY_SEPARATOR "?"
#define QUERY_PARAM_SEPARATOR '&'
#define KEY_VALUE_SEPARATOR "="

#define ONE_SECOND 1000
#define TIMEOUT_IN_SECONDS 30

#include "payloadnames.h"
