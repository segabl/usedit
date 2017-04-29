/*
 * ResourceHandler.cpp
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#include "ResourceManager.h"

#include "Utils.h"

using sf::Font;
using sf::RenderTexture;
using sf::Texture;
using std::map;
using std::string;

map<string, Font> ResourceManager::fonts;
map<string, Texture> ResourceManager::textures;

bool ResourceManager::initializeResources(string directory) {

  textures["note"].loadFromFile(directory + "data/note.png");
  textures["no_cover"].loadFromFile(directory + "data/no_cover.png");

  fonts["lyrics"].loadFromFile(directory + "data/lyrics.ttf");
  fonts["default"].loadFromFile(directory + "data/default.ttf");

  return true;
}

Font& ResourceManager::font(string name) {
  if (fonts.find(name) == fonts.end()) {
    log(1, "Font resource \"" + name + "\" does not exist and has been created!");
  }
  return fonts[name];
}

Texture& ResourceManager::texture(string name) {
  if (textures.find(name) == textures.end()) {
    log(1, "Texture resource \"" + name + "\" does not exist and has been created!");
  }
  return textures[name];
}
