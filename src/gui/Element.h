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
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>

namespace gui {

  class Element: public sf::Transformable, public sf::Drawable {
    protected:
      static Element* focus;
      static Element* hover;
      bool mouse_pressed;
      bool enabled;
      sf::Vector2f size;
      std::function<void(Element*)> callback;
      Element();
    public:
      static Element* focusedElement();

      bool isHovered() const;
      bool isFocused() const;

      bool isEnabled() const;
      void setEnabled(bool enabled);

      virtual std::function<void(Element*)> getCallback() const;
      virtual void setCallback(std::function<void(Element*)> callback);

      virtual sf::Vector2f getSize() const;
      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void update(sf::Vector2i mouse_pos);
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_ELEMENT_H_ */
