/*
 * Element.cpp
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#include "GuiElement.h"

#include <cmath>

std::set<gui::GuiElement*> gui::GuiElement::elements;
gui::GuiElement* gui::GuiElement::pressed_element;

gui::Settings gui::GuiElement::default_settings = {
    .text = gui::Params {
      .color = gui::Colors {
        .normal = sf::Color::White,
        .focus = sf::Color::White,
        .active = sf::Color::White,
        .disabled = sf::Color(255, 255, 255, 100)
      }
    },
    .background = gui::Params {
      .color = gui::Colors {
        .normal = sf::Color(90, 90, 90),
        .focus = sf::Color(120, 120, 120),
        .active = sf::Color(70, 70, 70),
        .disabled = sf::Color(90, 90, 90)
      }
    }
};

gui::GuiElement::GuiElement(sf::RenderWindow& window, sf::Vector2f size, bool enabled) :
    window(&window), state(enabled ? State::NORMAL : State::DISABLED), visible(true), size(size), parent(nullptr), settings(default_settings) {
  elements.insert(this);
}

gui::GuiElement::~GuiElement() {
  elements.erase(this);
  if (pressed_element == this) {
    pressed_element = nullptr;
  }
}

gui::GuiElement* gui::GuiElement::getParent() {
  return parent;
}

void gui::GuiElement::setParent(GuiElement* parent) {
  this->parent = parent;
}

bool gui::GuiElement::isParentEnabled() const {
  return !parent || parent->isEnabled();
}

bool gui::GuiElement::isParentVisible() const {
  return !parent || parent->isVisible();
}

gui::GuiElement* gui::GuiElement::handleMouseEvent(sf::Event& event) {
  sf::Vector2f mouse_pos;
  if (event.type == sf::Event::MouseMoved) {
    mouse_pos.x = event.mouseMove.x;
    mouse_pos.y = event.mouseMove.y;
  } else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
    mouse_pos.x = event.mouseButton.x;
    mouse_pos.y = event.mouseButton.y;
  }

  std::vector<Signal*> signals;
  for (auto element : elements) {
    if (!element->isEnabled() || !element->isVisible()) {
      continue;
    }
    bool mouse_inside = element->isInside(mouse_pos);
    State e_state = element->getState();
    if (event.type == sf::Event::MouseButtonPressed) {
      if (mouse_inside) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          if (e_state < State::ACTIVE) {
            signals.push_back(&element->onActiveGained());
          }
          signals.push_back(&element->onMouseLeftPressed());
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          signals.push_back(&element->onMouseRightPressed());
        }
      } else if (e_state == State::ACTIVE) {
        signals.push_back(&element->onActiveLost());
      }
    } else if (event.type == sf::Event::MouseButtonReleased) {
      if (mouse_inside) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          signals.push_back(&element->onMouseLeftReleased());
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          signals.push_back(&element->onMouseRightReleased());
        }
      }
    } else if (mouse_inside) {
      if (e_state < State::FOCUS) {
        signals.push_back(&element->onFocusGained());
      }
    } else {
      if (e_state >= State::FOCUS) {
        signals.push_back(&element->onFocusLost());
      }
    }

    if (mouse_inside && event.type != sf::Event::MouseButtonReleased) {
      if (event.type == sf::Event::MouseButtonPressed || pressed_element) {
        element->setState(State::ACTIVE);
        pressed_element = element;
      }
      element->setState(std::max(element->getState(), State::FOCUS));
    } else {
      if ((event.type == sf::Event::MouseButtonPressed && pressed_element == element) || event.type == sf::Event::MouseButtonReleased) {
        pressed_element = nullptr;
      }
      element->setState(mouse_inside ? State::FOCUS : State::NORMAL);
    }
  }
  for (auto signal : signals) {
    signal->send();
  }
  return pressed_element;
}

Signal& gui::GuiElement::onMouseLeftPressed() {
  return signals[0];
}

Signal& gui::GuiElement::onMouseRightPressed() {
  return signals[1];
}

Signal& gui::GuiElement::onMouseLeftReleased() {
  return signals[2];
}

Signal& gui::GuiElement::onMouseRightReleased() {
  return signals[3];
}

Signal& gui::GuiElement::onFocusGained() {
  return signals[4];
}

Signal& gui::GuiElement::onFocusLost() {
  return signals[5];
}

Signal& gui::GuiElement::onActiveGained() {
  return signals[6];
}

Signal& gui::GuiElement::onActiveLost() {
  return signals[7];
}

gui::GuiElement::State gui::GuiElement::getState() const {
  return state;
}

void gui::GuiElement::setState(State state) {
  this->state = state;
}

bool gui::GuiElement::isEnabled() const {
  return getState() > State::DISABLED && isParentEnabled();
}

void gui::GuiElement::setEnabled(bool enabled) {
  setState(enabled ? std::max(State::NORMAL, this->state) : State::DISABLED);
}

bool gui::GuiElement::isVisible() const {
  return visible && isParentVisible();
}

void gui::GuiElement::setVisible(bool visible) {
  this->visible = visible;
}

bool gui::GuiElement::isInside(sf::Vector2f point) const {
  sf::FloatRect rect(getPosition().x - getOrigin().x, getPosition().y - getOrigin().y, getSize().x, getSize().y);
  return rect.contains(point.x, point.y);
}

sf::Vector2f gui::GuiElement::getSize() const {
  return size;
}

void gui::GuiElement::setSize(sf::Vector2f size) {
  this->size = size;
}

void gui::GuiElement::setSize(float x, float y) {
  this->size = sf::Vector2f(x, y);
}

void gui::GuiElement::update() {
  if (!isVisible()) {
    return;
  }
  background.setSize(getSize());
  background.setPosition(getPosition());
  background.setOrigin(getOrigin());
  background.setScale(getScale());

  if (!isEnabled()) {
    background.setFillColor(settings.background.color.disabled);
  } else if (getState() == State::ACTIVE) {
    background.setFillColor(settings.background.color.active);
  } else if (getState() == State::FOCUS) {
    background.setFillColor(settings.background.color.focus);
  } else {
    background.setFillColor(settings.background.color.normal);
  }
}

void gui::GuiElement::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
}
