/*
 * Utils.h
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <sstream>
#include <string>
#include <vector>

void log(int level, std::string message);

std::vector<std::string> explode(std::string data, std::string delimiter);
std::string implode(std::vector<std::string> data, std::string delimiter);

template<class T>
std::string toString(T data) {
  std::stringstream converter;
  converter << data;
  std::string result = converter.str();
  return result;
}

template<class T>
T toType(std::string data) {
  std::stringstream converter(data);
  T result;
  converter >> result;
  return result;
}

template<class T>
std::vector<std::string> toString(std::vector<T> data) {
    std::vector<std::string> result;
  for (T type : data) {
    result.push_back(toString<T>(type));
  }
  return result;
}

template<class T>
std::vector<T> toType(std::vector<std::string> data) {
    std::vector<T> result;
  for (std::string type : data) {
    result.push_back(toType<T>(type));
  }
  return result;
}

#endif /* SRC_UTILS_H_ */
