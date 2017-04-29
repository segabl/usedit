/*
 * Element.h
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_ELEMENT_H_
#define SRC_GUI_ELEMENT_H_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>

namespace gui {

  class Element: public sf::Drawable {
    protected:
      static Element* focus;
      static Element* hover;
      sf::Vector2f position;
      sf::Vector2f size;
      sf::Vector2f origin;
      std::function<void()> callback;
      Element();
      bool isFocused() const;
      bool isHovered() const;
    public:
      std::function<void()> getCallback() const;
      void setCallback(std::function<void()> callback);

      sf::Vector2f getPosition() const;
      void setPosition(sf::Vector2f position);
      void setPosition(float x, float y);

      sf::Vector2f getSize() const;
      void setSize(sf::Vector2f size);
      void setSize(float x, float y);

      sf::Vector2f getOrigin() const;
      void setOrigin(sf::Vector2f origin);
      void setOrigin(float x, float y);

      virtual void update(sf::Vector2i mouse_pos);
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_ELEMENT_H_ */
