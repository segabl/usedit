/*
 * Button.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "Button.h"

#include <SFML/Window/Mouse.hpp>

#include "../ResourceManager.h"

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
  text.setPosition(getPosition() + (size * 0.5f - getOrigin()) + sf::Vector2f(0, isFocused()));
  text.setScale(getScale());
  text.setFillColor(ResourceManager::color("text") * (!isEnabled() ? sf::Color(255, 255, 255, 100) : sf::Color::White));
  rshape.setSize(size);
  rshape.setPosition(getPosition());
  rshape.setOrigin(getOrigin());
  rshape.setScale(getScale());
  sf::Color col = ResourceManager::color("interface");
  col.r += (!isEnabled() || isFocused()) * -20 + (isHovered() && !isFocused()) * 20;
  col.g += (!isEnabled() || isFocused()) * -20 + (isHovered() && !isFocused()) * 20;
  col.b += (!isEnabled() || isFocused()) * -20 + (isHovered() && !isFocused()) * 20;
  rshape.setFillColor(col);
}

void gui::Button::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  rt.draw(rshape, rs);
  rt.draw(text, rs);
}
