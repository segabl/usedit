/*
 * Song.h
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_SONG_H_
#define SRC_SONG_H_

#include "Note.h"

#include <audiere.h>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <map>
#include <list>
#include <string>

using audiere::AudioDevicePtr;
using audiere::OutputStreamPtr;
using sf::Texture;
using sf::Time;
using std::list;
using std::map;
using std::string;

typedef map<string, string> StringMap;
typedef list<Note*> NoteList;
typedef map<int, NoteList> NoteListMap;

class Song {
  private:
    static AudioDevicePtr audio_device;
    int sample_rate;
    OutputStreamPtr stream;
    bool paused;
  public:
    float bpm;
    float gap;
    float start;
    Texture cover;
    StringMap tags;
    NoteListMap note_tracks;
    Song();
    ~Song();
    bool loadFromFile(string fname);
    bool saveToFile(string fname) const;
    void clear();

    void play();
    void pause();
    void stop();

    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;

    void setPosition(Time time);
    Time getPosition() const;
    Time length() const;

    void fixPitches();
    void multiplyBPM(float mult);
};

#endif /* SRC_SONG_H_ */
