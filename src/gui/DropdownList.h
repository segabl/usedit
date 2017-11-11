/*
 * List.h
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_DROPDOWNLIST_H_
#define SRC_GUI_DROPDOWNLIST_H_

#include "Button.h"
#include "Container.h"

#include <vector>

namespace gui {

  class DropdownList: public Element {
    public:
      enum Direction {
        DOWN, UP
      };
    private:
      Button button;
      Container container;
      Direction direction;
    public:
      DropdownList(sf::RenderWindow& window, sf::Text text, sf::Vector2f size, Direction direction = DOWN, bool enabled = true);

      virtual sf::Vector2f getSize() const;
      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void setText(sf::Text text);
      virtual sf::Text getText() const;

      virtual void addElement(Element* element);
      virtual void removeElement(Element* element);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_DROPDOWNLIST_H_ */
