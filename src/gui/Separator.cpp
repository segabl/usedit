/*
 * Separator.cpp
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#include "Separator.h"

gui::Separator::Separator(sf::RenderWindow& window, sf::Vector2f size) :
    gui::GuiElement(window, size, true) {
}

void gui::Separator::update() {
  if (!visible) {
    return;
  }
  background.setSize(sf::Vector2f(size.x - 16, 2));
  background.setPosition(getPosition() + sf::Vector2f(8, size.y / 2 - 1));
  background.setOrigin(getOrigin());
  background.setScale(getScale());
  background.setFillColor(settings.text.color.disabled);
}

void gui::Separator::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!visible) {
    return;
  }
  rt.draw(background, rs);
}
