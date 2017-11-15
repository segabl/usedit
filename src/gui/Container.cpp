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
  clear(false);
}

void gui::Container::calculateSize() {
  unsigned col = 0;
  sf::Vector2f old_size = size;
  size = sf::Vector2f(0, 0);
  for (auto entry : elements) {
    auto e_size = entry.element->getSize();
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
  element->setZ(getZ() - 1);
  element->setParent(this);
  ElementEntry entry = { element };
  Signal* signal = &element->onResized();
  int signal_id = signal->connect(std::bind(gui::Container::calculateSize, this));
  entry.signal_tuples.push_back(std::tuple<Signal*, int>(signal, signal_id));
  signal = &element->onDestroyed();
  signal_id = signal->connect(std::bind(gui::Container::remove, this, element));
  entry.signal_tuples.push_back(std::tuple<Signal*, int>(signal, signal_id));
  elements.push_back(entry);
  calculateSize();
}

void gui::Container::remove(GuiElement* element) {
  for (auto it = elements.begin(); it != elements.end(); it++) {
    ElementEntry entry = *it;
    if (entry.element == element) {
      for (auto signal_tuple : entry.signal_tuples) {
        std::get<0>(signal_tuple)->disconnect(std::get<1>(signal_tuple));
      }
      entry.element->setParent(nullptr);
      elements.erase(it);
      break;
    }
  }
  calculateSize();
}

void gui::Container::clear(bool calc_size) {
  for (auto entry : elements) {
    for (auto signal_tuple : entry.signal_tuples) {
      std::get<0>(signal_tuple)->disconnect(std::get<1>(signal_tuple));
    }
    entry.element->setParent(nullptr);
  }
  elements.clear();
  if (calc_size) {
    calculateSize();
  }
}

void gui::Container::setZ(int z) {
  GuiElement::setZ(z);
  for (auto entry : elements) {
    entry.element->setZ(getZ() - 1);
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
  for (auto entry : elements) {
    auto e_size = entry.element->getSize();
    entry.element->setOrigin(0, 0);
    entry.element->setScale(getScale());
    if (columns == 0 || col < columns) {
      col += 1;
    } else {
      col = 1;
    }
    entry.element->setPosition(getPosition() - getOrigin() + pos);
    if (col >= columns && columns > 0) {
      pos.x = 0;
      pos.y += e_size.y;
    } else {
      pos.x += e_size.x;
    }
    entry.element->update();
  }
}

void gui::Container::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!isVisible()) {
    return;
  }
  rt.draw(background, rs);
  for (auto entry : elements) {
    rt.draw(*entry.element, rs);
  }
}
