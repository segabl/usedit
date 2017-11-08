/*
 * Utils.cpp
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#include "Utils.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <regex>
#include <windows.h>
#include <dirent.h>

void log(int level, std::string message) {
  std::ostream* out = &std::cout;
  if (level > 1) {
    out = &std::cerr;
  }
  const char* levels[] = { "INFO", "WARN", "ERROR", "FATAL" };
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  (*out) << std::put_time(&tm, "[%H:%M:%S]") << "[" << levels[(level >= 0 && level < 4) ? level : 0] << "] " << message << std::endl;
}

std::string getOpenFile(std::string title) {
  char filename[MAX_PATH];

  OPENFILENAME ofn;
  ZeroMemory(&filename, sizeof(filename));
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
  ofn.lpstrFilter = "UltraStar Song files\0*.txt\0";
  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrTitle = title.c_str();
  ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

  if (GetOpenFileNameA(&ofn)) {
    return filename;
  }
  return "";
}

std::vector<std::string> findFiles(std::string path, std::string pattern) {
  std::vector<std::string> files;
  DIR *dir;
  struct dirent *ent;
  std::string fname;
  std::smatch match;
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      fname = ent->d_name;
      if (pattern == "" || regex_search(fname, match, std::regex(pattern, std::regex::icase))) {
        files.push_back(path + "/" + fname);
      }
    }
    closedir(dir);
  }
  return files;
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
