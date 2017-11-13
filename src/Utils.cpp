/*
 * Utils.cpp
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#include "Utils.h"

#include <ctime>
#include <fstream>
#include <regex>
#include <windows.h>
#include <dirent.h>

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

bool fileExists(std::string filename) {
  return std::ifstream(filename).good();
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
