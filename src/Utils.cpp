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

std::vector<std::string> explode(std::string data, std::string delimiter) {
  std::vector<std::string> result;
  size_t pos;
  while ((pos = data.find(delimiter)) != std::string::npos) {
    result.push_back(data.substr(0, pos));
    data = data.substr(pos + delimiter.length());
  }
  if (data.size()) {
    result.push_back(data);
  }
  return result;
}

std::string implode(std::vector<std::string> data, std::string delimiter) {
  std::string result;
  for (std::string s : data) {
    result += s + delimiter;
  }
  return result.substr(0, result.length() - delimiter.length());
}
