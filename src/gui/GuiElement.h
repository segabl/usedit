/*
 * GuiElement.h
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_GUIELEMENT_H_
#define SRC_GUI_GUIELEMENT_H_

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
#include <set>

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

  typedef std::function<void()> CallbackFunction;
  class Callback {
    public:
      sf::Event::EventType type;
      sf::Mouse::Button button;
      CallbackFunction func;
      void set(sf::Event::EventType type, sf::Mouse::Button button, CallbackFunction func) {
        this->func = func;
        this->type = type;
        this->button = button;
      }
  };

  class GuiElement: public sf::Transformable, public sf::Drawable {
    public:
      enum State {
        DISABLED, NORMAL, FOCUS, ACTIVE
      };
    protected:
      static GuiElement* pressed_element;
      static std::set<GuiElement*> elements;
      sf::RenderWindow* window;
      State state;
      bool visible;
      sf::Vector2f size;
      sf::RectangleShape background;
      std::map<unsigned, Callback> callbacks;
      unsigned callbacks_id;
      GuiElement* parent;
      GuiElement(sf::RenderWindow& window, sf::Vector2f size, bool enabled = true);
      virtual ~GuiElement();
      virtual bool isParentEnabled() const;
      virtual bool isParentVisible() const;
    public:
      static Settings default_settings;
      Settings settings;

      static GuiElement* handleMouseEvent(sf::Event& event);

      virtual void setParent(GuiElement* parent);

      virtual State getState() const;
      virtual void setState(State state);

      virtual bool isEnabled() const;
      virtual void setEnabled(bool enabled);

      virtual bool isVisible() const;
      virtual void setVisible(bool visible);

      virtual bool isInside(sf::Vector2f point) const;

      virtual unsigned addCallback(sf::Event::EventType type, sf::Mouse::Button button, CallbackFunction func);
      virtual void removeCallback(unsigned id);

      virtual sf::Vector2f getSize() const;
      virtual void setSize(sf::Vector2f size);
      virtual void setSize(float x, float y);

      virtual void update();
      virtual void draw(sf::RenderTarget& rt, sf::RenderStates rs) const;
  };

} /* namespace gui */

#endif /* SRC_GUI_GUIELEMENT_H_ */
