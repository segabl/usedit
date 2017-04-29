/*
 * Song.cpp
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#include "Song.h"

#include "StringUtils.h"
#include "ResourceManager.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <regex>

using audiere::CreateTone;
using audiere::OpenDevice;
using audiere::OpenSampleSource;
using audiere::SampleFormat;
using audiere::SampleSourcePtr;
using sf::seconds;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::regex;
using std::smatch;

AudioDevicePtr Song::audio_device(OpenDevice());

Song::Song() {
  clear();
}

Song::~Song() {
  clear();
}

bool Song::loadFromFile(string fname) {
  clear();

  ifstream file;
  file.open(fname, ios_base::in);
  if (!file.is_open()) {
    cerr << "ERROR: Could not open \"" << fname << "\"!" << endl;
    return false;
  }

  int player = 0;

  string line;
  smatch match;
  while (true) {
    getline(file, line);

    if (regex_search(line, match, regex(R"(^#(.+?):(.+))", regex::icase))) {
      // Tags
    if (regex_match(match.str(1), regex(R"(^BPM$)", regex::icase))) {
      bpm = toType<float>(regex_replace(match.str(2), regex(R"(,)"), "."));
    } else if (regex_match(match.str(1), regex(R"(^GAP$)", regex::icase))) {
      gap = toType<float>(regex_replace(match.str(2), regex(R"(,)"), "."));
    } else if (regex_match(match.str(1), regex(R"(^START$)", regex::icase))) {
      start = toType<float>(regex_replace(match.str(2), regex(R"(,)"), "."));
    } else {
      tags[match.str(1)] = match.str(2);
    }
  } else if (regex_search(line, match, regex(R"(^([:F\*]) (-?[0-9]+) ([0-9]+) (-?[0-9]+) (.*))", regex::icase))) {
      // Notes
      Note::Type t = match.str(1) == "F" ? Note::FREESTYLE : (match.str(1) == "*" ? Note::GOLD : Note::DEFAULT);
      note_tracks[player].push_back(new Note(t, toType<int>(match.str(2)), toType<int>(match.str(3)), toType<int>(match.str(4)), match.str(5)));
    } else if (regex_search(line, match, regex(R"(^- ([0-9]+)(?: ([0-9]+))?)", regex::icase))) {
    // Linebreak
    note_tracks[player].push_back(new Note(Note::LINEBREAK, toType<int>(match.str(1))));
  } else if (regex_search(line, match, regex(R"(^P([0-9]))", regex::icase))) {
        // Player switch
        player = toType<int>(match.str(1));
      } else if (regex_search(line, match, regex(R"(^E)", regex::icase))) {
    // End of file
    file.close();
    break;
  } else {
    // Unrecognized line
    cerr << "WARNING: Unrecognized line \"" << line << "\"!" << endl;
  }
    if (file.eof()) {
      file.close();
      cerr << "ERROR: End of file reached without final \"E\" marker!" << endl;
      return false;
    }
  }

  cerr << "INFO: Parsed file successfully" << endl;

  string soundfile = regex_replace(fname, regex(R"([^/\\]+$)"), tags["MP3"]);
  SampleSourcePtr source = OpenSampleSource(soundfile.c_str());
  if (source) {
    int channel_count;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);
    if (audio_device) {
      stream = OpenSound(audio_device, source, true);
      setPosition(seconds(start));
    }
    cerr << "INFO: mp3 loaded" << endl;
  } else {
    cerr << "ERROR: Could not open \"" << soundfile << "\"!" << endl;
    return false;
  }

  if (tags["COVER"] != "") {
    cover.loadFromFile(regex_replace(fname, regex(R"([^/\\]+$)"), tags["COVER"]));
  }

  return true;
}

bool Song::saveToFile(string fname) const {
  ofstream file;
  file.open(fname, ios_base::out);
  if (!file.is_open()) {
    cerr << "ERROR: Could not open \"" << fname << "\"!" << endl;
    return false;
  }
  file << "#BPM:" << regex_replace(toString(bpm), regex(R"(\.)", regex::icase), ",") << endl;
  file << "#GAP:" << regex_replace(toString(gap), regex(R"(\.)", regex::icase), ",") << endl;
  if (start != 0) {
    file << "#START:" << regex_replace(toString(start), regex(R"(\.)", regex::icase), ",") << endl;
  }
  for (auto tag : tags) {
    if (tag.second != "") {
      file << "#" << tag.first << ":" << tag.second << endl;
    }
  }
  for (auto track : note_tracks) {
    if (track.first != 0) {
      file << "P" << track.first << endl;
    }
    for (auto note : track.second) {
      if (note->type == Note::LINEBREAK) {
        file << "- " << note->position;
        if (note->length != 0) {
          file << " " << note->length;
        }
        file << endl;
      } else {
        file << (note->type == Note::DEFAULT ? ":" : (note->type == Note::FREESTYLE ? "F" : "*")) << " ";
        file << note->position << " ";
        file << note->length << " ";
        file << note->pitch << " ";
        file << note->lyrics << endl;
      }
    }
  }
  file << "E";
  file.close();
  return true;
}

void Song::clear() {
  stop();
  sample_rate = 44100;
  stream = nullptr;
  paused = false;
  cover = ResourceManager::texture("no_cover");
  bpm = 0;
  gap = 0;
  start = 0;
  tags.clear();
  for (auto track : note_tracks) {
    for (auto note : track.second) {
      delete note;
    }
  }
  note_tracks.clear();
}

void Song::play() {
  if (stream) {
    paused = false;
    stream->play();
  }
}

void Song::pause() {
  if (stream) {
    paused = true;
    stream->stop();
  }
}

void Song::stop() {
  if (stream) {
    paused = false;
    stream->stop();
    stream->setPosition(start * sample_rate);
  }
}

bool Song::isPlaying() const {
  if (stream) {
    return stream->isPlaying();
  }
  return false;
}

bool Song::isPaused() const {
  return paused;
}

bool Song::isStopped() const {
  return !isPaused() && !isPlaying();
}

void Song::setPosition(Time time) {
  if (stream) {
    stream->setPosition(time.asSeconds() * sample_rate);
  }
}

Time Song::getPosition() const {
  if (stream) {
    return seconds(stream->getPosition() / float(sample_rate));
  }
  return Time();
}

Time Song::length() const {
  if (stream) {
    return seconds(stream->getLength() / float(sample_rate));
  }
  return Time();
}

void Song::fixPitches() {
  for (auto track : note_tracks) {
    float average = 0;
    for (auto note : track.second) {
      average += note->pitch;
    }
    average = (average / track.second.size()) / 12;
    if (abs(average) > 2) {
      for (auto note : track.second) {
        note->pitch += -round(average) * 12;
      }
    }
  }
}

void Song::multiplyBPM(float mult) {
  bpm *= mult;
  for (auto track : note_tracks) {
    for (auto note : track.second) {
      note->position *= mult;
      note->length *= mult;
    }
  }
}
