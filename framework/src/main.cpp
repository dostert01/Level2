#include <iostream>

#include "logger.h"

using namespace level2;
using namespace std;

int main() {
  Logger& logger = Logger::getInstance();
  logger.info("Hello from the logger");
  cout << "Hello Word" << endl;
  return 0;
}