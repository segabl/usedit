/*
 * ToneGenerator.cpp
 *
 *  Created on: 26.04.2017
 *      Author: Sebastian
 */

#include "ToneGenerator.h"

#include <cmath>
#include <iostream>

#include "Utils.h"

using audiere::AudioDevicePtr;

using audiere::SampleSourcePtr;
using audiere::CreateTone;
using audiere::CreateSquareWave;
using audiere::OpenDevice;
using audiere::OpenSound;
using sf::Clock;
using sf::Time;
using std::map;
using std::thread;
using std::tuple;
using std::get;

AudioDevicePtr ToneGenerator::audio_device(OpenDevice());

void ToneGenerator::work() {
  while (alive) {
    mtx.lock();
    if (stream && stream->isPlaying()) {
      stream->setVolume(volume * (type == 0 ? 1 : 0.25));
      if (clock.getElapsedTime() > time) {
        stream->stop();
      }
    }
    mtx.unlock();
  }
}

ToneGenerator::ToneGenerator(int type) :
    type(type), volume(1), alive(true) {
  float base_frequency = 440;
  float frequency_factor = pow(2.f, 1.f / 12.f);
  for (int i = -5 * 12; i < 6 * 12; i++) {
    tone_map[i] = OpenSound(audio_device, type == 0 ? CreateTone(base_frequency * pow(frequency_factor, i - 9)) : CreateSquareWave(base_frequency * pow(frequency_factor, i - 9)));
  }
  play_thread = thread(ToneGenerator::work, this);
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

void ToneGenerator::play(int pitch, Time time) {
  mtx.lock();
  stream = tone_map[pitch];
  this->time = time;
  if (stream) {
    clock.restart();
    stream->play();
  }
  mtx.unlock();
}

void ToneGenerator::stop() {
  mtx.lock();
  if (stream) {
    stream->stop();
  }
  mtx.unlock();
}

bool ToneGenerator::isPlaying() {
  return stream && stream->isPlaying();
}
