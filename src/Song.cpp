/*
 * Song.cpp
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#include "Song.h"

#include "Utils.h"
#include "ResourceManager.h"

#include <algorithm>
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
}

Song::~Song() {
  clear();
}

bool Song::loadFromFile(string fname) {
  clear();

  ifstream file(fname, ios_base::in);
  if (!file.is_open()) {
    log(2, "Could not open \"" + fname + "\"!");
    return false;
  }
  log(0, "Opened \"" + fname + "\"");

  int player = 0;
  Note prev_note;
  bool first = true;

  auto checkNoteOverlap = [](Note& n, Note& prev_note, bool& first) {
    if (!first && n.position < prev_note.position + prev_note.length) {
      log(1, n.type == Note::Type::LINEBREAK ? "Line break" : "Note", "at", n.position, "starts before",
          prev_note.type == Note::Type::LINEBREAK ? "line break of previous line!" : "previous note!");
    }
    first = false;
    prev_note = n;
  };

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
      } else if (regex_match(match.str(1), regex(R"(^MEDLEYSTARTBEAT$)", regex::icase))) {
        medley_start = toType<int>(regex_replace(match.str(2), regex(R"(,)"), "."));
      } else if (regex_match(match.str(1), regex(R"(^MEDLEYENDBEAT$)", regex::icase))) {
        medley_end = toType<int>(regex_replace(match.str(2), regex(R"(,)"), "."));
      } else {
        auto tag_name = match.str(1);
        std::transform(tag_name.begin(), tag_name.end(), tag_name.begin(), [](unsigned char c){ return std::toupper(c); });
        tags[tag_name] = match.str(2);
      }
    } else if (regex_search(line, match, regex(R"(^([:F\*]) (-?[0-9]+) ([0-9]+) (-?[0-9]+) (.*))", regex::icase))) {
      // Notes
      Note::Type t = match.str(1) == "F" ? Note::Type::FREESTYLE : (match.str(1) == "*" ? Note::Type::GOLD : Note::Type::DEFAULT);
      Note n(t, toType<int>(match.str(2)), toType<int>(match.str(3)), toType<int>(match.str(4)), match.str(5));
      note_tracks[player].push_back(n);
      checkNoteOverlap(n, prev_note, first);
      has_golden_notes = has_golden_notes || t == Note::Type::GOLD;
    } else if (regex_search(line, match, regex(R"(^- ([0-9]+)(?: ([0-9]+))?)", regex::icase))) {
      // Line break
      Note n(Note::Type::LINEBREAK, toType<int>(match.str(1)));
      checkNoteOverlap(n, prev_note, first);
      note_tracks[player].push_back(n);
    } else if (regex_search(line, match, regex(R"(^P([0-9]))", regex::icase))) {
      // Player switch
      first = true;
      player = toType<int>(match.str(1));
    } else if (regex_search(line, match, regex(R"(^E)", regex::icase))) {
      // End of file
      break;
    } else {
      // Unrecognized line
      log(1, "Unrecognized line \"" + line + "\"!");
    }
    if (file.eof()) {
      log(1, "End of file reached without final \"E\" marker!");
      break;
    }
  }

  log(0, "Song file parsed successfully");

  string path = regex_replace(fname, regex(R"([^/\\]+$)"), "");

  has_background = fileExists(path + tags["BACKGROUND"]);
  has_video = fileExists(path + tags["VIDEO"]);
  has_medley = medley_start < medley_end;

  string soundfile = path + tags["MP3"];
  SampleSourcePtr source = OpenSampleSource(soundfile.c_str());
  if (source) {
    int channel_count;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);
    if (audio_device && (stream = OpenSound(audio_device, source, true))) {
      song_len = seconds(stream->getLength() / float(sample_rate));
      setPosition(seconds(start));
    }
    log(0, "Audio file opened for streaming");
  } else {
    log(2, "Could not open \"" + soundfile + "\"!");
    clear();
    return false;
  }

  if (tags["COVER"] != "") {
    string coverfile = path + tags["COVER"];
    if (!cover.loadFromFile(coverfile)) {
      log(2, "Could not open \"" + coverfile + "\"!");
    }
  }
  loaded = true;
  this->fname = fname;
  return true;
}

bool Song::saveToFile(string fname) const {
  ofstream file(fname, ios_base::out);
  if (!file.is_open()) {
    log(2, "Could not open \"" + fname + "\"!");
    return false;
  }
  for (auto tag : tags) {
    if (tag.second != "") {
      file << "#" << tag.first << ":" << tag.second << endl;
    }
  }
  if (has_medley) {
    file << "#MEDLEYSTARTBEAT:" << toString(medley_start) << endl;
    file << "#MEDLEYENDBEAT:" << toString(medley_end) << endl;
  }
  if (start != 0) {
    file << "#START:" << regex_replace(toString(start), regex(R"(\.)", regex::icase), ",") << endl;
  }
  file << "#BPM:" << regex_replace(toString(bpm), regex(R"(\.)", regex::icase), ",") << endl;
  file << "#GAP:" << regex_replace(toString(gap), regex(R"(\.)", regex::icase), ",") << endl;
  for (auto track : note_tracks) {
    if (track.first != 0) {
      file << "P" << track.first << endl;
    }
    Note note;
    for (auto note = track.second.begin(); note != track.second.end(); note++) {
      if (note->type == Note::Type::LINEBREAK) {
        file << "- " << note->position;
        if (note->length != 0) {
          file << " " << note->length;
        }
        file << endl;
      } else {
        file << (note->type == Note::Type::DEFAULT ? ":" : (note->type == Note::Type::FREESTYLE ? "F" : "*")) << " ";
        file << note->position << " ";
        file << note->length << " ";
        file << note->pitch << " ";
        file << note->lyrics << endl;
      }
    }
  }
  file << "E";
  log(0, "Saved song to \"" + fname + "\"");
  return true;
}

void Song::clear() {
  stop();
  modified = false;
  fname = "";
  loaded = false;
  has_golden_notes = false;
  has_background = false;
  has_video = false;
  has_medley = false;
  sample_rate = 44100;
  stream = nullptr;
  paused = false;
  song_pos = Time();
  song_len = Time();
  cover = ResourceManager::texture("no_cover");
  bpm = 0;
  gap = 0;
  start = 0;
  medley_start = 0;
  medley_end = 0;
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
    song_pos = seconds(start);
  }
}

bool Song::isPlaying() const {
  return stream && stream->isPlaying();
}

bool Song::isPaused() const {
  return paused;
}

bool Song::isStopped() const {
  return !isPaused() && !isPlaying();
}

bool Song::isLoaded() const {
  return loaded;
}

bool Song::hasGoldenNotes() const {
  return has_golden_notes;
}

bool Song::hasBackground() const {
  return has_background;
}

bool Song::hasVideo() const {
  return has_video;
}

bool Song::hasMedley() const {
  return has_medley;
}

void Song::setPosition(Time time) {
  if (stream) {
    stream->setPosition(time.asSeconds() * sample_rate);
    song_pos = time;
  }
}

Time Song::getPosition() {
  if (isPlaying()) {
    song_pos = seconds(stream->getPosition() / float(sample_rate));
  }
  return song_pos;
}

Time Song::length() const {
  return song_len;
}
