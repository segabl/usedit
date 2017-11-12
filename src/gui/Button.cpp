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
    gui::GuiElement(window, size, enabled), text(text) {
}

void gui::Button::setText(sf::Text text) {
  this->text = text;
}

sf::Text gui::Button::getText() const {
  return this->text;
}

void gui::Button::update() {
  if (!isVisible()) {
    return;
  }
  gui::GuiElement::update();

  sf::FloatRect bounds = text.getLocalBounds();
  text.setOrigin(bounds.width / 2 + bounds.left, bounds.height / 2 + bounds.top);
  text.setPosition(getPosition() + (size * 0.5f - getOrigin()) + sf::Vector2f(0, getState() == State::ACTIVE));
  text.setScale(getScale());
  text.setFillColor(ResourceManager::color("text") * (!isEnabled() ? sf::Color(255, 255, 255, 100) : sf::Color::White));

  if (!isEnabled()) {
    text.setFillColor(settings.text.color.disabled);
  } else if (getState() == State::ACTIVE) {
    text.setFillColor(settings.text.color.active);
  } else if (getState() == State::FOCUS) {
    text.setFillColor(settings.text.color.focus);
  } else {
    text.setFillColor(settings.text.color.normal);
  }
}

void gui::Button::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!isVisible()) {
    return;
  }
  rt.draw(background, rs);
  rt.draw(text, rs);
}
