#pragma once

#include <string>

namespace level2 {

class Base64Encoder {
    public:
        static std::string encodeWithDefaultNewLines(std::string input);
        static std::string encodeNoNewLines(std::string input);
        static std::string encode(std::string input, int iMaxLineLen);
    private:
        static int DEFAULT_LINE_LEN;
        static const char base64LookupTable[];
        static void appendEncodedToOutput(int& iCurrentLineLen, int iMaxLineLen, std::string& output, std::string result);
        static void appendNewLineIfRequired(int& iCurrentLineLen, int iMaxLineLen, std::string& output);
        static std::string base64EncodeThreeChars(const char* input);
};
} //namespace level2 