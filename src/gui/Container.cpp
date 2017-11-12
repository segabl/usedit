/*
 * Container.cpp
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#include "Container.h"

#include <cmath>

gui::Container::Container(sf::RenderWindow& window, unsigned columns, bool enabled) :
    gui::GuiElement(window, sf::Vector2f(0, 0), enabled), columns(columns) {
}

void gui::Container::calculateSize() {
  unsigned col = 0;
  size = sf::Vector2f(0, 0);
  for (auto element : elements) {
    auto e_size = element->getSize();
    if (columns == 0 || col < columns) {
      col += 1;
      size.x += e_size.x;
      size.y = std::max(size.y, e_size.y);
    } else {
      col = 1;
      size.x = std::max(size.x, e_size.x);
      size.y += e_size.y;
    }
  }
}

void gui::Container::addElement(GuiElement* element) {
  elements.push_back(element);
  element->setParent(this);
  calculateSize();
}

void gui::Container::removeElement(GuiElement* element) {
  for (auto it = elements.begin(); it != elements.end(); it++) {
    if (*it == element) {
      elements.erase(it);
      return;
    }
  }
  element->setParent(nullptr);
  calculateSize();
}

void gui::Container::setSize(sf::Vector2f size) {
}

void gui::Container::setSize(float x, float y) {
}

void gui::Container::update() {
  if (!isVisible()) {
    return;
  }
  gui::GuiElement::update();

  if (!isEnabled()) {
    background.setFillColor(settings.background.color.disabled);
  } else {
    background.setFillColor(settings.background.color.normal);
  }

  unsigned col = 0;
  sf::Vector2f pos(0, 0);
  for (auto element : elements) {
    auto e_size = element->getSize();
    element->setOrigin(0, 0);
    element->setScale(getScale());
    if (columns == 0 || col < columns) {
      col += 1;
    } else {
      col = 1;
    }
    element->setPosition(getPosition() - getOrigin() + pos);
    if (col >= columns && columns > 0) {
      pos.x = 0;
      pos.y += e_size.y;
    } else {
      pos.x += e_size.x;
    }
    element->update();
  }
}

void gui::Container::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!isVisible()) {
    return;
  }
  rt.draw(background, rs);
  for (auto element : elements) {
    rt.draw(*element, rs);
  }
}
