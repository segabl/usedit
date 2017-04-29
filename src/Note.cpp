/*
 * Note.cpp
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#include "Note.h"

Note::Note(Type type, int position, int length, int pitch, string lyrics) {
  this->type = type;
  this->position = position;
  this->length = length;
  this->pitch = pitch;
  this->lyrics = lyrics;
}

bool Note::operator<(const Note& rhs) const {
  return position < rhs.position || (position == rhs.position && type == Note::LINEBREAK);
}
