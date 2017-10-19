/*
 * Button.h
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_BUTTON_H_
#define SRC_GUI_BUTTON_H_

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Element.h"

namespace gui {

  class Button: public Element {
    protected:
      sf::Text text;
      sf::RectangleShape rshape;
    public:
      Button(sf::Text text, sf::Vector2f size, std::function<void(Element*)> callback = nullptr);

      virtual void setText(sf::Text text);
      virtual sf::Text getText() const;

      virtual void update(sf::Vector2i mouse_pos);
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_BUTTON_H_ */
