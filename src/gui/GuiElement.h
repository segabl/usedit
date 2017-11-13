/*
 * GuiElement.h
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_GUIELEMENT_H_
#define SRC_GUI_GUIELEMENT_H_

#include "Signal.h"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <map>
#include <vector>

namespace gui {

  struct Colors {
      sf::Color normal, focus, active, disabled;
  };

  struct Params {
      Colors color;
  };

  struct Settings {
      Params text, background;
  };

  class GuiElement: public sf::Transformable, public sf::Drawable {
    public:
      enum State {
        DISABLED, NORMAL, FOCUS, ACTIVE
      };
    protected:
      static std::vector<GuiElement*> all_elements;
      static GuiElement* active_element;

      State state;
      bool visible;
      int z;
      sf::Vector2f size;
      sf::RectangleShape background;
      GuiElement* parent;
      GuiElement(sf::Vector2f size, bool enabled = true);
      virtual ~GuiElement();
      virtual bool isParentEnabled() const;
      virtual bool isParentVisible() const;
      Signal signals[10];

      static void sortElements();
    public:
      static Settings default_settings;
      Settings settings;

      static GuiElement* handleMouseEvent(sf::Event& event);

      virtual Signal& onMouseLeftPressed();
      virtual Signal& onMouseRightPressed();
      virtual Signal& onMouseLeftReleased();
      virtual Signal& onMouseRightReleased();
      virtual Signal& onFocusGained();
      virtual Signal& onFocusLost();
      virtual Signal& onActiveGained();
      virtual Signal& onActiveLost();

      virtual GuiElement* getParent();
      virtual void setParent(GuiElement* parent);

      virtual int getZ();
      virtual void setZ(int z);

      virtual State getState() const;
      virtual void setState(State state);

      virtual bool isEnabled() const;
      virtual void setEnabled(bool enabled);

      virtual bool isVisible() const;
      virtual void setVisible(bool visible);

      virtual bool isInside(sf::Vector2f point) const;

      virtual sf::Vector2f getSize() const;
      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_GUIELEMENT_H_ */
