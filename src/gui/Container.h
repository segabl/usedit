/*
 * Container.h
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_CONTAINER_H_
#define SRC_GUI_CONTAINER_H_

#include <map>
#include <vector>
#include <tuple>

#include <SFML/Graphics/RectangleShape.hpp>

#include "GuiElement.h"

namespace gui {

  struct ElementEntry {
      GuiElement* element;
      std::vector<std::tuple<Signal*, int>> signal_tuples;
  };

  class Container: public GuiElement {
    protected:
      unsigned columns;
      std::vector<ElementEntry> elements;
      void calculateSize();
    public:
      Container(unsigned columns = 1, bool enabled = true);
      virtual ~Container();

      virtual void add(GuiElement* element);
      virtual void remove(GuiElement* element);
      virtual void clear(bool calc_size = true);

      virtual void setZ(int z);

      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_CONTAINER_H_ */
