/*
 * Utils.h
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define BEATS_TO_SECONDS(BEATS, BPM) ((BEATS) / ((BPM) / 60.f) / 4.f))
#define SECONDS_TO_BEATS(SECONDS, BPM) (((BPM) / 60.f) * (SECONDS) * 4.f)

template<class T>
void print(std::ostream* out, T type) {
  (*out) << type << std::endl;
}

template<class H, class ... T>
void print(std::ostream* out, H head, T ... tail) {
  (*out) << head << " ";
  print(out, tail...);
}

template<class ...L>
void log(int level, L ... args) {
  std::ostream* out = &std::cout;
  if (level > 1) {
    out = &std::cerr;
  }
  const char* levels[] = { "INFO", "WARN", "ERROR", "FATAL" };
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  (*out) << std::put_time(&tm, "[%H:%M:%S]") << "[" << levels[(level >= 0 && level < 4) ? level : 0] << "] ";
  print(out, args...);
}

std::string getOpenFile(std::string title);

std::vector<std::string> findFiles(std::string path, std::string pattern = "");

bool fileExists(std::string filename);

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
