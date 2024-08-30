#include <iostream>

#include "logger/logger.h"

using namespace second_take;
using namespace std;

int main() {
  Logger& logger = Logger::getInstance();
  logger.info("Hello from the logger");
  cout << "Hello Word" << endl;
  return 0;
}