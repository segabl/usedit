/*
 * Element.cpp
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#include "Element.h"

#include <SFML/Window/Mouse.hpp>

gui::Element* gui::Element::focus = nullptr;
gui::Element* gui::Element::hover = nullptr;

gui::Element* gui::Element::focusedElement() {
  return focus;
}

gui::Element::Element() :
    mouse_pressed(false), enabled(true), size(0, 0), callback(nullptr) {
}

bool gui::Element::isHovered() const {
  return hover == this;
}

bool gui::Element::isFocused() const {
  return focus == this;
}

bool gui::Element::isEnabled() const {
  return enabled;
}

void gui::Element::setEnabled(bool enabled) {
  this->enabled = enabled;
}

std::function<void(gui::Element*)> gui::Element::getCallback() const {
  return callback;
}

void gui::Element::setCallback(std::function<void(Element*)> callback) {
  this->callback = callback;
}

sf::Vector2f gui::Element::getSize() const {
  return size;
}

void gui::Element::setSize(sf::Vector2f size) {
  this->size = size;
}

void gui::Element::setSize(float x, float y) {
  this->size = sf::Vector2f(x, y);
}

void gui::Element::update(sf::Vector2i mouse_pos) {
  if (!enabled) {
    return;
  }
  bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left) != mouse_pressed && sf::Mouse::isButtonPressed(sf::Mouse::Left);
  bool released = sf::Mouse::isButtonPressed(sf::Mouse::Left) != mouse_pressed && !sf::Mouse::isButtonPressed(sf::Mouse::Left);
  mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

  sf::FloatRect rect(getPosition().x - getOrigin().x, getPosition().y - getOrigin().y, getSize().x, getSize().y);
  if (rect.contains(mouse_pos.x, mouse_pos.y)) {
    if (!hover) {
      hover = this;
    }
    if (pressed && !focus) {
      focus = this;
    }
    if (released && focus == this && callback) {
      callback(this);
    }
  } else if (hover) {
    hover = nullptr;
  }
  if (!mouse_pressed && focus == this) {
    focus = nullptr;
  }
}

void gui::Element::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
}
