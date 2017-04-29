/*
 * ResourceHandler.cpp
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#include "ResourceManager.h"

map<string, Font> ResourceManager::fonts;
map<string, Texture> ResourceManager::textures;
map<string, RenderTexture> ResourceManager::render_textures;

bool ResourceManager::initializeResources(string directory) {

  textures["note"].loadFromFile(directory + "data/note.png");
  textures["no_cover"].loadFromFile(directory + "data/no_cover.png");

  fonts["lyrics"].loadFromFile(directory + "data/lyrics.ttf");
  fonts["default"].loadFromFile(directory + "data/default.ttf");

  return true;
}

Font& ResourceManager::font(string name) {
  return fonts[name];
}

Texture& ResourceManager::texture(string name) {
  return textures[name];
}

RenderTexture& ResourceManager::renderTexture(string name) {
  return render_textures[name];
}
