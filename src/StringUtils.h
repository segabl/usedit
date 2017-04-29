/*
 * StringUtils.h
 *
 *  Created on: 30.12.2016
 *      Author: Sebastian
 */

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::stringstream;
using std::vector;

vector<string> explode(string data, string delimiter);
string implode(vector<string> data, string delimiter);

template<class T>
string toString(T data) {
  stringstream converter;
  converter << data;
  string result = converter.str();
  return result;
}

template<class T>
T toType(string data) {
  stringstream converter(data);
  T result;
  converter >> result;
  return result;
}

template<class T>
vector<string> toString(vector<T> data) {
  vector<string> result;
  for (T type : data) {
    result.push_back(toString<T>(type));
  }
  return result;
}

template<class T>
vector<T> toType(vector<string> data) {
  vector<T> result;
  for (string type : data) {
    result.push_back(toType<T>(type));
  }
  return result;
}

#endif /* STRINGUTILS_H_ */
