/*
 * StringUtils.cpp
 *
 *  Created on: 30.12.2016
 *      Author: Sebastian
 */

#include "StringUtils.h"

vector<string> explode(string data, string delimiter) {
  vector<string> result;
  size_t pos;
  while ((pos = data.find(delimiter)) != string::npos) {
    result.push_back(data.substr(0, pos));
    data = data.substr(pos + delimiter.length());
  }
  if (data.size()) {
    result.push_back(data);
  }
  return result;
}

string implode(vector<string> data, string delimiter) {
  string result;
  for (string s : data) {
    result += s + delimiter;
  }
  return result.substr(0, result.length() - delimiter.length());
}
