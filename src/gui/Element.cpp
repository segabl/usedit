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

gui::Element::Element() :
    position(0, 0), size(0, 0), origin(0, 0), callback(nullptr) {
}

void gui::Element::update(sf::Vector2i mouse_pos) {
  if (mouse_pos.x > position.x && mouse_pos.x < position.x + size.x && mouse_pos.y > position.y && mouse_pos.y < position.y + size.y) {
    if (!hover) {
      hover = this;
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !focus) {
      focus = this;
    }
  } else if (isHovered()) {
    hover = nullptr;
  }
  if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && isFocused()) {
    focus = nullptr;
    if (callback) {
      callback();
    }
  }
}

bool gui::Element::isFocused() const {
  return this == focus;
}

bool gui::Element::isHovered() const {
  return this == hover;
}

std::function<void()> gui::Element::getCallback() const {
  return callback;
}

void gui::Element::setCallback(std::function<void()> callback) {
  this->callback = callback;
}

sf::Vector2f gui::Element::getPosition() const {
  return position;
}

void gui::Element::setPosition(sf::Vector2f position) {
  this->position = position;
}

void gui::Element::setPosition(float x, float y) {
  this->position = sf::Vector2f(x, y);
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

sf::Vector2f gui::Element::getOrigin() const {
  return origin;
}

void gui::Element::setOrigin(sf::Vector2f origin) {
  this->origin = origin;
}

void gui::Element::setOrigin(float x, float y) {
  this->origin = sf::Vector2f(x, y);
}

void gui::Element::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
}
