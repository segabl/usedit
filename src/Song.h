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
#include <sol.hpp>

#include <map>
#include <list>

typedef std::list<Note> NoteList;
typedef std::map<std::string, std::string> StringMap;
typedef std::map<int, NoteList> NoteListMap;

class Song {
  private:
    static audiere::AudioDevicePtr audio_device;
    int sample_rate;
    audiere::OutputStreamPtr stream;
    bool paused;
    sol::state lua;
    bool loaded;
    bool has_golden_notes;
    bool has_background;
    bool has_video;
    bool has_medley;
  public:
    bool modified;
    std::string fname;
    float bpm;
    float gap;
    float start;
    sf::Texture cover;
    StringMap tags;
    NoteListMap note_tracks;
    Song();
    ~Song();
    bool loadFromFile(std::string fname);
    bool saveToFile(std::string fname) const;
    void clear();

    void play();
    void pause();
    void stop();

    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;

    bool isLoaded() const;

    bool hasGoldenNotes() const;
    bool hasBackground() const;
    bool hasVideo() const;
    bool hasMedley() const;

    void setPosition(sf::Time time);
    sf::Time getPosition() const;
    sf::Time length() const;

    bool executeLuaFile(std::string fname);
};

#endif /* SRC_SONG_H_ */
