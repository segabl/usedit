/*
 * ResourceHandler.cpp
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#include "ResourceManager.h"

#include "Utils.h"

using sf::Color;
using sf::Font;
using sf::RenderTexture;
using sf::Texture;
using std::unordered_map;
using std::string;

unordered_map<string, Font> ResourceManager::fonts;
unordered_map<string, Texture> ResourceManager::textures;
unordered_map<string, Color> ResourceManager::colors;

bool ResourceManager::initializeResources(string directory) {

  textures["note"].loadFromFile(directory + "data/note.png");
  textures["no_cover"].loadFromFile(directory + "data/no_cover.png");
  textures["icons"].loadFromFile(directory + "data/icons.png");

  fonts["lyrics"].loadFromFile(directory + "data/lyrics.ttf");
  fonts["default"].loadFromFile(directory + "data/default.ttf");

  colors["background"] = Color(20, 20, 20);
  colors["interface"] = Color(90, 90, 90);
  colors["text"] = Color(255, 255, 255);

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

Color& ResourceManager::color(string name) {
  if (colors.find(name) == colors.end()) {
    log(1, "Color resource \"" + name + "\" does not exist and has been created!");
  }
  return colors[name];
}
