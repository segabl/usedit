/*
 * Element.h
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_ELEMENT_H_
#define SRC_GUI_ELEMENT_H_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>

namespace gui {

  struct Settings {
      struct Params {
          struct Colors {
              sf::Color normal, hovered, focused, disabled;
          };
          Colors color;
      };
      Params text, background;
  };

  class Element: public sf::Transformable, public sf::Drawable {
    protected:
      static Element* focus;
      static Element* hover;
      sf::RenderWindow* window;
      bool mouse_pressed;
      bool enabled;
      bool visible;
      sf::Vector2f size;
      sf::RectangleShape rshape;
      std::function<void(Element*)> callback;
      Element(sf::RenderWindow& window, sf::Vector2f size, bool enabled = true);
    public:
      static Settings default_settings;
      Settings settings;

      static Element* focusedElement();

      bool isHovered() const;
      bool isFocused() const;

      bool isEnabled() const;
      void setEnabled(bool enabled);

      bool isVisible() const;
      void setVisible(bool visible);

      virtual std::function<void(Element*)> getCallback() const;
      virtual void setCallback(std::function<void(Element*)> callback);

      virtual sf::Vector2f getSize() const;
      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_ELEMENT_H_ */
