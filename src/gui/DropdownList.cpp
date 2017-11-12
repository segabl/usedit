/*
 * List.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "DropdownList.h"

#include <SFML/Window/Mouse.hpp>

gui::DropdownList::DropdownList(sf::RenderWindow& window, sf::Text text, sf::Vector2f size, gui::DropdownList::Direction direction, bool enabled) :
    gui::GuiElement(window, size, enabled), button(window, text, size), container(window, 1), direction(direction) {
  container.setVisible(false);
  button.addCallback(sf::Event::MouseButtonPressed, sf::Mouse::Left, [&]() {
    container.setVisible(!container.isVisible());
  });
  container.setParent(this);
  button.setParent(this);
}

bool gui::DropdownList::isInside(sf::Vector2f point) const {
  return button.isInside(point);
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

void gui::DropdownList::addElement(GuiElement* element) {
  container.addElement(element);
}

void gui::DropdownList::removeElement(GuiElement* element) {
  container.removeElement(element);
}

void gui::DropdownList::update() {
  if (!isVisible()) {
    return;
  }
  if (container.isVisible()) {
    button.setState(State::ACTIVE);
  }
  button.setPosition(getPosition());
  button.setOrigin(getOrigin());
  button.setScale(getScale());
  button.update();

  container.setPosition(getPosition() - getOrigin() + sf::Vector2f(0, direction == UP ? -container.getSize().y : size.y));
  container.setOrigin(0, 0);
  container.setScale(getScale());
  container.update();
}

void gui::DropdownList::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  if (!isVisible()) {
    return;
  }
  rt.draw(container, rs);
  rt.draw(button, rs);
}
