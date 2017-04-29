/*
 * NoteList.h
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_NOTELIST_H_
#define SRC_NOTELIST_H_

#include "Note.h"

class NoteList {
  private:
    Note* _begin;
    Note* _end;
    size_t _size;
  public:
    NoteList();
    ~NoteList();

    void add(Note* note);
    void calcLineEnds();

    Note* begin();
    Note* end();

    size_t size() const;
};

#endif /* SRC_NOTELIST_H_ */
