/*
 * Container.h
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_CONTAINER_H_
#define SRC_GUI_CONTAINER_H_

#include <SFML/Graphics/RectangleShape.hpp>

#include <vector>
#include "GuiElement.h"

namespace gui {

  class Container: public GuiElement {
    protected:
      unsigned columns;
      std::vector<GuiElement*> elements;
      void calculateSize();
    public:
      Container(sf::RenderWindow& window, unsigned columns = 1, bool enabled = true);

      virtual void addElement(GuiElement* element);
      virtual void removeElement(GuiElement* element);

      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_CONTAINER_H_ */
