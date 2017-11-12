/*
 * Button.h
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_BUTTON_H_
#define SRC_GUI_BUTTON_H_

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "GuiElement.h"

namespace gui {

  class Button: public GuiElement {
    protected:
      sf::Text text;
    public:
      Button(sf::RenderWindow& window, sf::Text text, sf::Vector2f size, bool enabled = true);

      virtual void setText(sf::Text text);
      virtual sf::Text getText() const;

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_BUTTON_H_ */
