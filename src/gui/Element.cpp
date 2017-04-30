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
    enabled(true), size(0, 0), callback(nullptr) {
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
  if (mouse_pos.x > getPosition().x - getOrigin().x && mouse_pos.x < getPosition().x - getOrigin().x + size.x && mouse_pos.y > getPosition().y - getOrigin().y
      && mouse_pos.y < getPosition().y - getOrigin().y + size.y) {
    if (!hover) {
      hover = this;
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !focus) {
      focus = this;
    }
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && focus == this && callback) {
      callback(this);
    }
  } else if (hover) {
    hover = nullptr;
  }
  if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && focus == this) {
    focus = nullptr;
  }
}

void gui::Element::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
}
