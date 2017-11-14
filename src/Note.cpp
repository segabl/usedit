/*
 * Note.cpp
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#include "Note.h"

#include "Utils.h"

std::string Note::getName(int pitch) {
  const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
  int nr = pitch / 12;
  if (pitch < 0) {
    pitch += (-nr + 1) * 12;
    nr--;
  }
  pitch = pitch % 12;
  return std::string(names[pitch]) + toString(nr + 4);
}

Note::Note(Type type, int position, int length, int pitch, std::string lyrics) {
  this->type = type;
  this->position = position;
  this->length = length;
  this->pitch = pitch;
  this->lyrics = lyrics;
}

bool Note::operator<(const Note& rhs) const {
  return position < rhs.position || (position == rhs.position && type == Type::LINEBREAK);
}
