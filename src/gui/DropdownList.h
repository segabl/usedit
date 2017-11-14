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

  class DropdownList: public Button {
    public:
      enum Direction {
        DOWN, UP
      };
    private:
      Container list;
      Direction direction;
    public:
      DropdownList(sf::Text text, sf::Vector2f size, Direction direction = DOWN, bool enabled = true);

      virtual void add(GuiElement* element);
      virtual void remove(GuiElement* element);
      virtual void clear();

      virtual void setZ(int z);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_DROPDOWNLIST_H_ */
