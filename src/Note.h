/*
 * Note.h
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_NOTE_H_
#define SRC_NOTE_H_

#include <string>

class Note {
  public:
    Note* next;
    Note* prev;
    Note* line_start;
    Note* line_end;
    enum Type {
      LINEBREAK = -1, DEFAULT, FREESTYLE, GOLD,
    };
    Type type;
    int position;
    int length;
    int pitch;
    std::string lyrics;
    Note(Type type = DEFAULT, int position = 0, int length = 0, int pitch = 0, std::string lyrics = "");
    bool operator<(const Note& rhs) const;
};

#endif /* SRC_NOTE_H_ */
