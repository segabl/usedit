/*
 * Note.h
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_NOTE_H_
#define SRC_NOTE_H_

#include <string>

using std::string;

class Note {
  public:
    enum Type {
      LINEBREAK = -1, DEFAULT, FREESTYLE, GOLD,
    };
    Type type;
    int position;
    int length;
    int pitch;
    string lyrics;
    Note(Type type = DEFAULT, int position = 0, int length = 0, int pitch = 0, string lyrics = "");
    bool operator<(const Note& rhs) const;
};

#endif /* SRC_NOTE_H_ */
