/*
 * Container.cpp
 *
 *  Created on: 11.11.2017
 *      Author: Sebastian
 */

#include "Container.h"

#include <cmath>

gui::Container::Container(unsigned columns, bool enabled) :
    gui::GuiElement(sf::Vector2f(0, 0), enabled), columns(columns) {
}

gui::Container::~Container() {
  clear();
}

void gui::Container::calculateSize() {
  unsigned col = 0;
  sf::Vector2f old_size = size;
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
  if (old_size != size) {
    onResized().send();
  }
}

void gui::Container::add(GuiElement* element) {
  elements.push_back(element);
  element->setZ(getZ() - 1);
  element->setParent(this);
  Signal* signal = &element->onResized();
  int signal_id = signal->connect(std::bind(gui::Container::calculateSize, this));
  element_signals[element].push_back(std::tuple<Signal*, int>(signal, signal_id));
  signal = &element->onDestroyed();
  signal_id = signal->connect(std::bind(gui::Container::remove, this, element));
  element_signals[element].push_back(std::tuple<Signal*, int>(signal, signal_id));
  calculateSize();
}

void gui::Container::remove(GuiElement* element) {
  for (auto it = elements.begin(); it != elements.end(); it++) {
    if (*it == element) {
      elements.erase(it);
      break;
    }
  }
  element->setParent(nullptr);
  for (auto signal_tuple : element_signals[element]) {
    std::get<0>(signal_tuple)->disconnect(std::get<1>(signal_tuple));
  }
  element_signals.erase(element);
  calculateSize();
}

void gui::Container::clear() {
  for (auto element : element_signals) {
    element.first->setParent(nullptr);
    for (auto signal_tuple : element.second) {
      std::get<0>(signal_tuple)->disconnect(std::get<1>(signal_tuple));
    }
  }
  elements.clear();
  element_signals.clear();
  calculateSize();
}

void gui::Container::setZ(int z) {
  GuiElement::setZ(z);
  for (auto element : elements) {
    element->setZ(getZ() - 1);
  }
}

void gui::Container::setSize(sf::Vector2f size) {
}

void gui::Container::setSize(float x, float y) {
}

void gui::Container::update() {
  if (!isVisible()) {
    return;
  }
  GuiElement::update();

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
