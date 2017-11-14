/*
 * Element.cpp
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#include "GuiElement.h"

#include <cmath>

std::vector<gui::GuiElement*> gui::GuiElement::all_elements;
gui::GuiElement* gui::GuiElement::active_element;

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

void gui::GuiElement::sortElements() {
  auto comp = [](GuiElement* lhs, GuiElement* rhs) -> bool {
    return lhs->getZ() < rhs->getZ();
  };
  std::sort(all_elements.begin(), all_elements.end(), comp);
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

  sortElements();

  std::vector<Signal*> signals;
  GuiElement* mouse_element = nullptr;
  GuiElement* top_element = nullptr;
  for (auto e : all_elements) {
    if (!e->isEnabled() || !e->isVisible()) {
      continue;
    }
    bool mouse_inside = e->isInside(mouse_pos);
    State e_state = e->getState();

    bool ignore = !mouse_element || e == mouse_element->getParent();

    // Handle signals
    if (event.type == sf::Event::MouseButtonPressed) {
      if (mouse_inside && ignore) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          if (e_state < State::ACTIVE) {
            signals.push_back(&e->onActiveGained());
          }
          signals.push_back(&e->onMouseLeftPressed());
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          signals.push_back(&e->onMouseRightPressed());
        }
        e->setState(State::ACTIVE);
      } else if (e_state == State::ACTIVE) {
        signals.push_back(&e->onActiveLost());
        e->setState(State::NORMAL);
      }
    } else if (event.type == sf::Event::MouseButtonReleased) {
      if (mouse_inside && ignore) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          signals.push_back(&e->onMouseLeftReleased());
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          signals.push_back(&e->onMouseRightReleased());
        }
        e->setState(State::FOCUS);
      } else {
        e->setState(State::NORMAL);
      }
    } else if (mouse_inside && ignore) {
      if (e_state < State::FOCUS) {
        signals.push_back(&e->onFocusGained());
        e->setState(active_element ? State::ACTIVE : State::FOCUS);
      }
    } else {
      if (e_state >= State::FOCUS) {
        signals.push_back(&e->onFocusLost());
        e->setState(State::NORMAL);
      }
    }

    if (mouse_inside && ignore) {
      mouse_element = e;
      if (!top_element) {
        top_element = e;
      }
    }
  }

  if (event.type == sf::Event::MouseButtonReleased) {
    active_element = nullptr;
  } else if (event.type == sf::Event::MouseButtonPressed) {
    active_element = top_element;
  }

  for (auto it = signals.rbegin(); it!= signals.rend(); it++) {
    (*it)->send();
  }

  return top_element;
}

gui::GuiElement::GuiElement(sf::Vector2f size, bool enabled) :
    state(enabled ? State::NORMAL : State::DISABLED), visible(true), z(0), size(size), parent(nullptr), settings(default_settings) {
  all_elements.push_back(this);
}

gui::GuiElement::~GuiElement() {
  if (active_element == this) {
    active_element = nullptr;
  }
  for (auto it = all_elements.begin(); it != all_elements.end(); it++) {
    if (*it == this) {
      all_elements.erase(it);
      return;
    }
  }
  onDestroyed().send();
}

bool gui::GuiElement::isParentEnabled() const {
  return !parent || parent->isEnabled();
}

bool gui::GuiElement::isParentVisible() const {
  return !parent || parent->isVisible();
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

Signal& gui::GuiElement::onResized() {
  return signals[8];
}

Signal& gui::GuiElement::onDestroyed() {
  return signals[9];
}

gui::GuiElement* gui::GuiElement::getParent() {
  return parent;
}

void gui::GuiElement::setParent(GuiElement* parent) {
  this->parent = parent;
}

int gui::GuiElement::getZ() {
  return z;
}

void gui::GuiElement::setZ(int z) {
  this->z = z;
}

gui::State gui::GuiElement::getState() const {
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
  if (this->size != size) {
    this->size = size;
    onResized().send();
  }
}

void gui::GuiElement::setSize(float x, float y) {
  this->setSize(sf::Vector2f(x, y));
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
