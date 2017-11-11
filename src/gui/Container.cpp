/*
 * Container.cpp
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#include "Container.h"

#include <cmath>

gui::Container::Container(sf::RenderWindow& window, unsigned columns, std::vector<Element*> elements, bool enabled) :
    gui::Element(window, sf::Vector2f(0, 0), enabled), columns(columns), elements(elements) {
  calculateSize();
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

void gui::Container::addElement(Element* element) {
  elements.push_back(element);
  calculateSize();
}

void gui::Container::removeElement(Element* element) {
  for (auto it = elements.begin(); it != elements.end(); it++) {
    if (*it == element) {
      elements.erase(it);
      return;
    }
  }
  calculateSize();
}

void gui::Container::setSize(sf::Vector2f size) {
}

void gui::Container::setSize(float x, float y) {
}

void gui::Container::update() {
  if (!visible) {
    return;
  }

  rshape.setSize(size);
  rshape.setPosition(getPosition());
  rshape.setOrigin(getOrigin());
  rshape.setScale(getScale());
  if (!isEnabled()) {
    rshape.setFillColor(settings.background.color.disabled);
  } else if (isFocused()) {
    rshape.setFillColor(settings.background.color.focused);
  } else if (isHovered()) {
    rshape.setFillColor(settings.background.color.hovered);
  } else {
    rshape.setFillColor(settings.background.color.normal);
  }

  unsigned col = 0;
  sf::Vector2f pos(0, 0);
  for (auto element : elements) {
    auto e_size = element->getSize();
    if (columns == 0 || col < columns) {
      col += 1;
      element->setPosition(getPosition() - getOrigin() + element->getOrigin() + pos);
      pos.x += e_size.x;
    } else {
      col = 1;
      pos.x = 0;
      pos.y += e_size.y;
      element->setPosition(getPosition() - getOrigin() + element->getOrigin() + pos);
      pos.x += e_size.x;
    }
    element->update();
  }
}

void gui::Container::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!visible) {
    return;
  }
  rt.draw(rshape, rs);
  for (auto element : elements) {
    rt.draw(*element, rs);
  }
}
