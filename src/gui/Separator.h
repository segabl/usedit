/*
 * Separator.h
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_SEPARATOR_H_
#define SRC_GUI_SEPARATOR_H_

#include "GuiElement.h"

namespace gui {

  class Separator: public GuiElement {
    public:
      Separator(sf::Vector2f size);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_SEPARATOR_H_ */
