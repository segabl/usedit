/*
 * Container.h
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_CONTAINER_H_
#define SRC_GUI_CONTAINER_H_

#include <SFML/Graphics/RectangleShape.hpp>

#include "Element.h"

#include <vector>

namespace gui {

  class Container: public Element {
    protected:
      unsigned columns;
      std::vector<Element*> elements;
      void calculateSize();
    public:
      Container(sf::RenderWindow& window, unsigned columns = 1, std::vector<Element*> elements = std::vector<Element*>(), bool enabled = true);

      virtual void addElement(Element* element);
      virtual void removeElement(Element* element);

      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_CONTAINER_H_ */
