/*
 * List.cpp
 *
 *  Created on: 30.04.2017
 *      Author: Sebastian
 */

#include "DropdownList.h"

#include <cmath>
#include <SFML/Window/Mouse.hpp>

gui::DropdownList::DropdownList(sf::Text text, sf::Vector2f size, gui::DropdownList::Direction direction) :
    button(text, size, [&](Element*) {show_list = !show_list;}), direction(direction), show_list(false) {
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
  elements.push_back(element);
}

void gui::DropdownList::removeElement(Element* element) {
  for (auto it = elements.begin(); it != elements.end(); it++) {
    if (*it == element) {
      elements.erase(it);
      return;
    }
  }
}

void gui::DropdownList::update(sf::Vector2i mouse_pos) {
  if (!enabled) {
    return;
  }
  bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left) != mouse_pressed && sf::Mouse::isButtonPressed(sf::Mouse::Left);
  bool released = sf::Mouse::isButtonPressed(sf::Mouse::Left) != mouse_pressed && !sf::Mouse::isButtonPressed(sf::Mouse::Left);
  mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

  size = button.getSize();
  if (show_list) {
    for (Element* element : elements) {
      size.x = std::max(size.x, element->getSize().x);
      size.y += element->getSize().y;
      element->setPosition(getPosition() + element->getOrigin() + sf::Vector2f(0, size.y * (direction == UP ? -1 : 1) + button.getSize().y * (direction == UP ? 1 : -1)));
      element->setScale(getScale());
      element->update(mouse_pos);
    }
  }

  sf::FloatRect rect_full(getPosition().x - getOrigin().x, getPosition().y - getOrigin().y - size.y * (direction == UP) + button.getSize().y * (direction == UP), size.x, size.y);
  sf::FloatRect rect(getPosition().x - getOrigin().x, getPosition().y - getOrigin().y - size.y, button.getSize().x, button.getSize().y);
  if ((!rect_full.contains(mouse_pos.x, mouse_pos.y) && pressed) || (!rect.contains(mouse_pos.x, mouse_pos.y) && released)) {
    show_list = false;
  }

  button.setEnabled(isEnabled());
  button.setPosition(getPosition());
  button.setOrigin(getOrigin());
  button.setScale(getScale());
  button.update(mouse_pos);
}

void gui::DropdownList::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
  button.draw(rt, rs);
  if (show_list) {
    for (Element* element : elements) {
      element->draw(rt, rs);
    }
  }
}
