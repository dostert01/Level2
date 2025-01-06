#include <iostream>

#include "logger/logger.h"

using namespace event_forge;
using namespace std;

int main() {
  Logger& logger = Logger::getInstance();
  logger.info("Hello from the logger");
  cout << "Hello Word" << endl;
  return 0;
}