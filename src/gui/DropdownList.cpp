/*
 * List.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "DropdownList.h"

gui::DropdownList::DropdownList(sf::Text text, sf::Vector2f size, gui::DropdownList::Direction direction, bool enabled) :
    gui::Button(text, size, enabled), list(1), direction(direction) {
  onMouseLeftPressed().connect([&]() {
    list.setVisible(true);
  });
  list.setParent(this);
  list.setZ(getZ() - 1);
  list.setVisible(false);
  list.onActiveLost().connect([&]() {
    list.setVisible(false);
  });
  list.onMouseLeftReleased().connect([&]() {
    list.setVisible(false);
  });
}

void gui::DropdownList::addElement(GuiElement* element) {
  list.addElement(element);
}

void gui::DropdownList::removeElement(GuiElement* element) {
  list.removeElement(element);
}

void gui::DropdownList::update() {
  if (!isVisible()) {
    return;
  }
  if (list.isVisible()) {
    setState(State::ACTIVE);
    list.setState(State::ACTIVE);
  }
  Button::update();

  list.setPosition(getPosition() - getOrigin() + sf::Vector2f(0, direction == UP ? -list.getSize().y : size.y));
  list.setOrigin(0, 0);
  list.setScale(getScale());
  list.update();
}

void gui::DropdownList::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!isVisible()) {
    return;
  }
  Button::draw(rt, rs);
  rt.draw(list, rs);
}
