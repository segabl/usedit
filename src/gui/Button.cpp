/*
 * Button.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "Button.h"

#include <SFML/Window/Mouse.hpp>

gui::Button::Button(sf::Text text, sf::Vector2f size, std::function<void(Element*)> callback) :
    text(text) {
  this->size = size;
  this->callback = callback;
}

void gui::Button::setText(sf::Text text) {
  this->text = text;
}

sf::Text gui::Button::getText() const {
  return this->text;
}


void gui::Button::update(sf::Vector2i mouse_pos) {
  gui::Element::update(mouse_pos);

  sf::FloatRect bounds = text.getLocalBounds();
  text.setOrigin(bounds.width / 2 + bounds.left, bounds.height / 2 + bounds.top);
  text.setPosition(getPosition() + (size - getOrigin()) * 0.5f + sf::Vector2f(0, isFocused()));
  text.setScale(getScale());
  text.setFillColor(isEnabled() ? sf::Color(255, 255, 255) : sf::Color(100, 100, 100));
  rshape.setSize(size);
  rshape.setPosition(getPosition());
  rshape.setScale(getScale());
  rshape.setFillColor(isFocused() || !isEnabled() ? sf::Color(30, 30, 30) : (isHovered() ? sf::Color(60, 60, 60) : sf::Color(40, 40, 40)));
}

void gui::Button::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  rt.draw(rshape, rs);
  rt.draw(text, rs);
}
