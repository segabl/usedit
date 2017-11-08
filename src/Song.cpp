/*
 * Song.cpp
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#include "Song.h"

#include "Utils.h"
#include "ResourceManager.h"

#include <cmath>
#include <fstream>
#include <regex>

using audiere::CreateTone;
using audiere::OpenDevice;
using audiere::OpenSampleSource;
using audiere::SampleFormat;
using audiere::SampleSourcePtr;
using audiere::AudioDevicePtr;
using audiere::OutputStreamPtr;
using sf::Texture;
using sf::Time;
using sf::seconds;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::regex;
using std::smatch;
using std::list;
using std::map;
using std::string;
using std::max;
using std::min;

AudioDevicePtr Song::audio_device(OpenDevice());

Song::Song() {
  clear();
  lua.open_libraries();
  lua["log"] = static_cast<void (*)(int, std::string)>(log);
  lua.new_usertype<Song>("Song",
      "bpm", &Song::bpm,
      "gap", &Song::gap,
      "tracks", &Song::note_tracks
  );
  lua.new_usertype<Note>("Note",
      sol::constructors<Note(Note::Type, int, int, int, std::string)>(),
      "type", &Note::type,
      "position", &Note::position,
      "length", &Note::length,
      "pitch", &Note::pitch,
      "lyrics", &Note::lyrics
  );
  lua.new_enum("NoteType",
      "LINEBREAK", Note::LINEBREAK,
      "DEFAULT", Note::DEFAULT,
      "FREESTYLE", Note::FREESTYLE,
      "GOLD", Note::GOLD
  );
}

Song::~Song() {
  clear();
}

bool Song::loadFromFile(string fname) {
  clear();

  ifstream file;
  file.open(fname, ios_base::in);
  if (!file.is_open()) {
    log(2, "Could not open \"" + fname + "\"!");
    return false;
  }

  int player = 0;

  string line;
  smatch match;
  while (true) {
    getline(file, line);

    if (regex_search(line, match, regex(R"(^#(.+?):(.+))", regex::icase))) {
      // Tags
    if (regex_match(match.str(1), regex(R"(^BPM$)", regex::icase)))
{      bpm = toType<float>(regex_replace(match.str(2), regex(R"(,)"), "."));
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
      note_tracks[player].push_back(Note(t, toType<int>(match.str(2)), toType<int>(match.str(3)), toType<int>(match.str(4)), match.str(5)));
    } else if (regex_search(line, match, regex(R"(^- ([0-9]+)(?: ([0-9]+))?)", regex::icase))) {
    // Line break
    note_tracks[player].push_back(Note(Note::LINEBREAK, toType<int>(match.str(1))));
  } else if (regex_search(line, match, regex(R"(^P([0-9]))", regex::icase))) {
        // Player switch
        player = toType<int>(match.str(1));
      } else if (regex_search(line, match, regex(R"(^E)", regex::icase))) {
    // End of file
    for (auto track: note_tracks) {
      track.second.sort();
    }
    file.close();
    break;
  } else {
    // Unrecognized line
    log(1, "Unrecognized line \"" + line + "\"!");
  }
    if (file.eof()) {
      file.close();
      log(2, "End of file reached without final \"E\" marker!");
      clear();
      return false;
    }
  }

  log(0, "Song file parsed successfully");
  log(0, tags["ARTIST"] + " - " + tags["TITLE"] + ", BPM: " + toString(bpm) + ", GAP: " + toString(gap));

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
    log(0, "Song audio file loaded");
  } else {
    log(2, "Could not open \"" + soundfile + "\"!");
    clear();
    return false;
  }

  if (tags["COVER"] != "") {
    string coverfile = regex_replace(fname, regex(R"([^/\\]+$)"), tags["COVER"]);
    if (!cover.loadFromFile(coverfile)) {
      log(2, "Could not open \"" + coverfile + "\"!");
    }
  }
  this->fname = fname;
  return true;
}

bool Song::saveToFile(string fname) const {
  ofstream file;
  file.open(fname, ios_base::out);
  if (!file.is_open()) {
    log(2, "Could not open \"" + fname + "\"!");
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
    Note note;
    for (auto note = track.second.begin(); note != track.second.end(); note++) {
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
  log(0, "Saved song to \"" + fname + "\"");
  return true;
}

void Song::clear() {
  stop();
  modified = false;
  fname = "";
  sample_rate = 44100;
  stream = nullptr;
  paused = false;
  cover = ResourceManager::texture("no_cover");
  bpm = 0;
  gap = 0;
  start = 0;
  tags.clear();
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

bool Song::executeLuaFile(string fname) {
  lua["song"] = this;
  try {
    lua.script_file(fname);
  } catch (sol::error &e) {
    log(2, e.what());
    return false;
  }
  return true;
}
