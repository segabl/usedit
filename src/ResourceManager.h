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

#include <map>
#include <string>

class ResourceManager {
  private:
    ResourceManager();
    ResourceManager(ResourceManager&);

    static std::map<std::string, sf::Font> fonts;
    static std::map<std::string, sf::Texture> textures;
  public:
    static bool initializeResources(std::string directory);

    static sf::Font& font(std::string name);
    static sf::Texture& texture(std::string name);
};

#endif /* SRC_RESOURCEMANAGER_H_ */
