#pragma once

#define READ_BUFFER_SIZE 2049
#define MAX_READ (READ_BUFFER_SIZE - 1)
#define LINE_TERMINATOR "\r\n"
#define HEADER_TERMINATOR std::string("\r\n\r\n").c_str()
#define HEADER_FIELD_SEPARATOR ": "
#define METHOD_PATH_SEPARATOR " /"
#define PATH_PROTOCOL_SEPARATOR " HTTP/1.1"
#define PATH_QUERY_SEPARATOR "?"
#define QUERY_PARAM_SEPARATOR '&'
#define KEY_VALUE_SEPARATOR "="

#define ONE_SECOND 1000
#define TIMEOUT_IN_SECONDS 30
