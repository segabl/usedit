/*
 * Signal.cpp
 *
 *  Created on: 12.11.2017
 *      Author: Sebastian
 */

#include "Signal.h"

Signal::Signal() :
    slot_id(0) {
}

int Signal::connect(std::function<void()> function) {
  slots[slot_id] = function;
  return slot_id++;
}

void Signal::disconnect(int id) {
  slots.erase(id);
}

void Signal::send() {
  for (auto entry : slots) {
    entry.second();
  }
}
