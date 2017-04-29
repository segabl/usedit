/*
 * ResourceHandler.h
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_RESOURCEMANAGER_H_
#define SRC_RESOURCEMANAGER_H_

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <map>
#include <string>

using sf::Font;
using sf::RenderTexture;
using sf::Texture;

using std::map;
using std::string;

class ResourceManager {
  private:
    ResourceManager();
    ResourceManager(ResourceManager&);

    static map<string, Font> fonts;
    static map<string, Texture> textures;
    static map<string, RenderTexture> render_textures;
  public:
    static bool initializeResources(string directory);

    static Font& font(string name);
    static Texture& texture(string name);
    static RenderTexture& renderTexture(string name);
};

#endif /* SRC_RESOURCEMANAGER_H_ */
