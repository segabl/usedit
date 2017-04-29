/*
 * ToneGenerator.cpp
 *
 *  Created on: 26.04.2017
 *      Author: Sebastian
 */

#include "ToneGenerator.h"

#include <cmath>

using audiere::CreateTone;
using audiere::CreateSquareWave;
using audiere::OpenDevice;
using audiere::OpenSound;

AudioDevicePtr ToneGenerator::audio_device(OpenDevice());

ToneGenerator::ToneGenerator(int type) : type(type), volume(1) {
  float base_frequency = 440;
  float frequency_factor = pow(2.f, 1.f / 12.f);
  for (int i = -5 * 12; i < 6 * 12; i++) {
    tone_map[i] = type == 0 ? CreateTone(base_frequency * pow(frequency_factor, i - 9)) : CreateSquareWave(base_frequency * pow(frequency_factor, i - 9));
  }
}

void ToneGenerator::setVolume(float volume) {
  this->volume = volume;
  if (stream) {
    stream->setVolume(volume * (type == 0 ? 1 : 0.25));
  }
}

float ToneGenerator::getVolume() const {
  return volume;
}

void ToneGenerator::play(int pitch) {
  if (stream) {
    stream->stop();
  }
  if (tone_map[pitch]) {
    stream = OpenSound(audio_device, tone_map[pitch]);
    stream->setVolume(volume * (type == 0 ? 1 : 0.25));
    if (stream) {
      stream->play();
    }
  }
}

void ToneGenerator::stop() {
  if (stream) {
    stream->stop();
  }
}

bool ToneGenerator::isPlaying() const {
  return stream and stream->isPlaying();
}
