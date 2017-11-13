/*
 * ToneGenerator.cpp
 *
 *  Created on: 26.04.2017
 *      Author: Sebastian
 */

#include "ToneGenerator.h"

#include <cmath>

audiere::AudioDevicePtr ToneGenerator::audio_device(audiere::OpenDevice());
std::map<int, OutputStreamPtrMap> ToneGenerator::tone_map;

void ToneGenerator::work() {
  while (alive) {
    stream_mtx.lock();
    if (stream && stream->isPlaying()) {
      stream->setVolume(volume * (type == 0 ? 1 : 0.25));
      if (clock.getElapsedTime() > time) {
        stream->stop();
      }
    }
    stream_mtx.unlock();
  }
  stop();
}

ToneGenerator::ToneGenerator(int type) :
    type(type), volume(1), alive(true) {
  if (tone_map[type].empty()) {
    float base_frequency = 440;
    float frequency_factor = std::pow(2.f, 1.f / 12.f);
    for (int i = -5 * 12; i < 6 * 12; i++) {
      tone_map[type][i] = audiere::OpenSound(audio_device,
          type == 0 ? audiere::CreateTone(base_frequency * std::pow(frequency_factor, i - 9)) : audiere::CreateSquareWave(base_frequency * std::pow(frequency_factor, i - 9)));
    }
  }
  play_thread = std::thread(ToneGenerator::work, this);
}

ToneGenerator::~ToneGenerator() {
  alive = false;
  if (play_thread.joinable()) {
    play_thread.join();
  }
}

void ToneGenerator::setVolume(float volume) {
  this->volume = volume;
}

void ToneGenerator::play(int pitch, sf::Time time) {
  stream_mtx.lock();
  stream = tone_map[type][pitch];
  this->time = time;
  if (stream) {
    clock.restart();
    stream->play();
  }
  stream_mtx.unlock();
}

void ToneGenerator::stop() {
  stream_mtx.lock();
  if (stream) {
    stream->stop();
  }
  stream_mtx.unlock();
}

bool ToneGenerator::isPlaying() {
  return stream && stream->isPlaying();
}
