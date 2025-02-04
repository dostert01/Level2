#include <string.h>
#include "base64.h"

namespace level2 {

const char Base64Encoder::base64LookupTable[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64Encoder::DEFAULT_LINE_LEN = 76;

std::string Base64Encoder::encodeWithDefaultNewLines(std::string input) {
    return encode(input, Base64Encoder::DEFAULT_LINE_LEN);
}

std::string Base64Encoder::encodeNoNewLines(std::string input) {
    return encode(input, -1);
}

std::string Base64Encoder::encode(std::string input, int iMaxLineLen) {
  char toBeEncoded[4];
  std::string output = "";
  const char* inBuffer = input.c_str();
  char* currentPosition = (char*)inBuffer;
  char* endPosition = (char*)inBuffer + strlen(inBuffer);
  int iCurrentLineLen = 0;

  while (currentPosition != endPosition) {
    memset(toBeEncoded, 0, 4);
    int i;
    for (i = 0; i < 3; i++) {
      if (currentPosition >= endPosition) break;
      toBeEncoded[i] = *currentPosition++;
    }
    if (i != 0) {
      appendEncodedToOutput(iCurrentLineLen, iMaxLineLen, output,
                            base64EncodeThreeChars(toBeEncoded));
    }
  }
  return output;
}

std::string Base64Encoder::base64EncodeThreeChars(const char* input) {
    char output[] = "====";
    int inputLength = (strlen(input) < 3) ? strlen(input) : 3;
    
	output[0] = base64LookupTable[(input[0]) >> 2];
	output[1] = base64LookupTable[(((input[0]) & 3) << 4) | (input[1] >> 4)];
    if (inputLength > 1)
	    output[2] = base64LookupTable[((input[1] & 15) << 2) | (input[2] >> 6)];
	if (inputLength > 2)
        output[3] = base64LookupTable[input[2] & 63];

	return std::string(output);
}

void Base64Encoder::appendNewLineIfRequired(int& iCurrentLineLen, int iMaxLineLen, std::string& output) {
  if (iCurrentLineLen++ == iMaxLineLen) {
    output.append("\n");
    iCurrentLineLen = 1;
  }
}

void Base64Encoder::appendEncodedToOutput(int& iCurrentLineLen, int iMaxLineLen, std::string& output, std::string result) {
  for (int pos = 0; pos < 4; pos++) {
    appendNewLineIfRequired(iCurrentLineLen, iMaxLineLen, output);
    output.append(result.substr(pos, 1));
  }
}

} // namespace level2 