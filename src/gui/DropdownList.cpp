/*
 * List.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "DropdownList.h"

#include <cmath>
#include <SFML/Window/Mouse.hpp>

gui::DropdownList::DropdownList(sf::RenderWindow& window, sf::Text text, sf::Vector2f size, gui::DropdownList::Direction direction, bool enabled) :
    gui::Element(window, size, enabled), button(window, text, size, enabled), container(window, 1), direction(direction) {
  container.setVisible(false);
  button.setCallback([&](Element*) {
    container.setVisible(!container.isVisible());
  });
}

sf::Vector2f gui::DropdownList::getSize() const {
  return button.getSize();
}

void gui::DropdownList::setSize(sf::Vector2f size) {
  button.setSize(size);
}

void gui::DropdownList::setSize(float x, float y) {
  button.setSize(x, y);
}

void gui::DropdownList::setText(sf::Text text) {
  button.setText(text);
}

sf::Text gui::DropdownList::getText() const {
  return button.getText();
}

void gui::DropdownList::addElement(Element* element) {
  container.addElement(element);
}

void gui::DropdownList::removeElement(Element* element) {
  container.removeElement(element);
}

void gui::DropdownList::update() {
  container.setEnabled(enabled);
  if (!visible) {
    container.setVisible(false);
  }
  container.setPosition(getPosition() + sf::Vector2f(0, direction == UP ? -container.getSize().y : size.y));
  container.setOrigin(getOrigin());
  container.setScale(getScale());
  container.update();

  button.setEnabled(enabled);
  button.setVisible(visible);
  button.setPosition(getPosition());
  button.setOrigin(getOrigin());
  button.setScale(getScale());
  button.update();

  if (enabled && visible && focus != &button) {
    bool window_focus = window->hasFocus();
    bool released = window_focus && sf::Mouse::isButtonPressed(sf::Mouse::Left) != mouse_pressed && !sf::Mouse::isButtonPressed(sf::Mouse::Left);
    mouse_pressed = window_focus && sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (released) {
      container.setVisible(false);
    }
  }
}

void gui::DropdownList::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!visible) {
    return;
  }
  button.draw(rt, rs);
  container.draw(rt, rs);
}
