/*
 * Element.cpp
 *
 *  Created on: 28.04.2017
 *      Author: Sebastian
 */

#include "GuiElement.h"

#include <cmath>

gui::GuiElement* gui::GuiElement::pressed_element;
std::set<gui::GuiElement*> gui::GuiElement::elements;
gui::Settings gui::GuiElement::default_settings = { .text = gui::Params { .color = gui::Colors { .normal = sf::Color::White, .focus = sf::Color::White, .active = sf::Color::White,
    .disabled = sf::Color(255, 255, 255, 100) } }, .background = gui::Params { .color = gui::Colors { .normal = sf::Color(90, 90, 90), .focus = sf::Color(120, 120, 120), .active =
    sf::Color(70, 70, 70), .disabled = sf::Color(90, 90, 90) } } };

gui::GuiElement::GuiElement(sf::RenderWindow& window, sf::Vector2f size, bool enabled) :
    window(&window), state(enabled ? State::NORMAL : State::DISABLED), visible(true), size(size), callbacks_id(0), parent(nullptr), settings(default_settings) {
  elements.insert(this);
}

gui::GuiElement::~GuiElement() {
  elements.erase(this);
}

void gui::GuiElement::setParent(GuiElement* parent) {
  this->parent = parent;
}

bool gui::GuiElement::isParentEnabled() const {
  return !parent || parent->isEnabled();
}

bool gui::GuiElement::isParentVisible() const {
  return !parent || parent->isVisible();
}

gui::GuiElement* gui::GuiElement::handleMouseEvent(sf::Event& event) {
  sf::Vector2f mouse_pos;
  if (event.type == sf::Event::MouseMoved) {
    mouse_pos.x = event.mouseMove.x;
    mouse_pos.y = event.mouseMove.y;
  } else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
    mouse_pos.x = event.mouseButton.x;
    mouse_pos.y = event.mouseButton.y;
  }
  GuiElement* prev_pressed_element = pressed_element;
  for (auto element : elements) {
    if (!element->isEnabled() || !element->isVisible()) {
      continue;
    }
    bool mouse_inside = element->isInside(mouse_pos);
    if (mouse_inside && event.type != sf::Event::MouseButtonReleased) {
      if (event.type == sf::Event::MouseButtonPressed || pressed_element) {
        element->setState(State::ACTIVE);
        pressed_element = element;
      }
      element->setState(std::max(element->getState(), State::FOCUS));
    } else {
      if ((event.type == sf::Event::MouseButtonPressed && pressed_element == element) || event.type == sf::Event::MouseButtonReleased) {
        pressed_element = nullptr;
      }
      element->setState(mouse_inside ? State::FOCUS : State::NORMAL);
    }
    for (auto clbk : element->callbacks) {
      if (mouse_inside && clbk.second.type == event.type && clbk.second.button == event.mouseButton.button) {
        clbk.second.func();
      }
    }
  }
  return pressed_element ? pressed_element : prev_pressed_element;
}

gui::GuiElement::State gui::GuiElement::getState() const {
  return state;
}

void gui::GuiElement::setState(State state) {
  this->state = state;
}

bool gui::GuiElement::isEnabled() const {
  return getState() > State::DISABLED && isParentEnabled();
}

void gui::GuiElement::setEnabled(bool enabled) {
  setState(enabled ? std::max(State::NORMAL, this->state) : State::DISABLED);
}

bool gui::GuiElement::isVisible() const {
  return visible && isParentVisible();
}

void gui::GuiElement::setVisible(bool visible) {
  this->visible = visible;
}

bool gui::GuiElement::isInside(sf::Vector2f point) const {
  sf::FloatRect rect(getPosition().x - getOrigin().x, getPosition().y - getOrigin().y, getSize().x, getSize().y);
  return rect.contains(point.x, point.y);
}

unsigned gui::GuiElement::addCallback(sf::Event::EventType type, sf::Mouse::Button button, CallbackFunction func) {
  int id = callbacks_id++;
  this->callbacks[id].set(type, button, func);
  return id;
}

void gui::GuiElement::removeCallback(unsigned id) {
  this->callbacks.erase(id);
}

sf::Vector2f gui::GuiElement::getSize() const {
  return size;
}

void gui::GuiElement::setSize(sf::Vector2f size) {
  this->size = size;
}

void gui::GuiElement::setSize(float x, float y) {
  this->size = sf::Vector2f(x, y);
}

void gui::GuiElement::update() {
  if (!isVisible()) {
    return;
  }
  background.setSize(getSize());
  background.setPosition(getPosition());
  background.setOrigin(getOrigin());
  background.setScale(getScale());

  if (!isEnabled()) {
    background.setFillColor(settings.background.color.disabled);
  } else if (getState() == State::ACTIVE) {
    background.setFillColor(settings.background.color.active);
  } else if (getState() == State::FOCUS) {
    background.setFillColor(settings.background.color.focus);
  } else {
    background.setFillColor(settings.background.color.normal);
  }
}

void gui::GuiElement::draw(sf::RenderTarget& rt, sf::RenderStates rs) const {
}
