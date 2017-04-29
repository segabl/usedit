/*
 * NoteList.cpp
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#include "NoteList.h"

NoteList::NoteList() : _begin(nullptr), _end(nullptr), _size(0) {

}

NoteList::~NoteList() {

}

void NoteList::add(Note* note) {
  note->line_start = _end && _end->type != Note::LINEBREAK ? _end->line_start : note;
  if (!_begin) {
    _begin = note;
  }
  if (_end) {
    _end->next = note;
    note->prev = _end;
  }
  _end = note;
  _size++;
}

void NoteList::calcLineEnds() {
  Note* line_end = _end;
  for (Note* n = _end; n; n = n->prev) {
    if (n->type == Note::LINEBREAK) {
      line_end = n->prev;
    }
    n->line_end = line_end;
  }
}

Note* NoteList::begin() {
  return _begin;
}

Note* NoteList::end() {
  return _end;
}

size_t NoteList::size() const {
  return _size;
}
