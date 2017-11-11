/*
 * Button.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "Button.h"

#include <SFML/Window/Mouse.hpp>

#include "../ResourceManager.h"

gui::Button::Button(sf::RenderWindow& window, sf::Text text, sf::Vector2f size, bool enabled) :
    gui::Element(window, size, enabled), text(text) {
}

void gui::Button::setText(sf::Text text) {
  this->text = text;
}

sf::Text gui::Button::getText() const {
  return this->text;
}

void gui::Button::update() {
  if (!visible) {
    return;
  }
  gui::Element::update();

  sf::FloatRect bounds = text.getLocalBounds();
  text.setOrigin(bounds.width / 2 + bounds.left, bounds.height / 2 + bounds.top);
  text.setPosition(getPosition() + (size * 0.5f - getOrigin()) + sf::Vector2f(0, isFocused()));
  text.setScale(getScale());
  text.setFillColor(ResourceManager::color("text") * (!isEnabled() ? sf::Color(255, 255, 255, 100) : sf::Color::White));

  if (!isEnabled()) {
    text.setFillColor(settings.text.color.disabled);
  } else if (isFocused()) {
    text.setFillColor(settings.text.color.focused);
  } else if (isHovered()) {
    text.setFillColor(settings.text.color.hovered);
  } else {
    text.setFillColor(settings.text.color.normal);
  }
}

void gui::Button::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!visible) {
    return;
  }
  rt.draw(rshape, rs);
  rt.draw(text, rs);
}
