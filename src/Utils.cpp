/*
 * Utils.cpp
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#include "Utils.h"

#include <iostream>

void log(int level, std::string message) {
  std::ostream* out = &std::cout;
  if (level > 1) {
    out = &std::cerr;
  }
  const char* levels[] = {"INFO", "WARN", "ERROR"};
  (*out) << "[" << levels[level] << "] " << message << std::endl;
}
