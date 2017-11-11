/*
 * Button.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "Button.h"

#include <SFML/Window/Mouse.hpp>

#include "../ResourceManager.h"

gui::Button::Button(sf::RenderWindow* window, sf::Text text, sf::Vector2f size, bool enabled, std::function<void(Element*)> callback) :
    gui::Element(window, size, enabled, callback), text(text) {
}

void gui::Button::setText(sf::Text text) {
  this->text = text;
}

sf::Text gui::Button::getText() const {
  return this->text;
}

void gui::Button::update() {
  gui::Element::update();

  sf::FloatRect bounds = text.getLocalBounds();
  text.setOrigin(bounds.width / 2 + bounds.left, bounds.height / 2 + bounds.top);
  text.setPosition(getPosition() + (size * 0.5f - getOrigin()) + sf::Vector2f(0, isFocused()));
  text.setScale(getScale());
  text.setFillColor(ResourceManager::color("text") * (!isEnabled() ? sf::Color(255, 255, 255, 100) : sf::Color::White));
  rshape.setSize(size);
  rshape.setPosition(getPosition());
  rshape.setOrigin(getOrigin());
  rshape.setScale(getScale());
  if (!isEnabled()) {
    text.setFillColor(settings.text.color.disabled);
    rshape.setFillColor(settings.background.color.disabled);
  } else if (isFocused()) {
    text.setFillColor(settings.text.color.focused);
    rshape.setFillColor(settings.background.color.focused);
  } else if (isHovered()) {
    text.setFillColor(settings.text.color.hovered);
    rshape.setFillColor(settings.background.color.hovered);
  } else {
    text.setFillColor(settings.text.color.normal);
    rshape.setFillColor(settings.background.color.normal);
  }
}

void gui::Button::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  rt.draw(rshape, rs);
  rt.draw(text, rs);
}
