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

ToneGenerator::ToneGenerator(int type) {
  float base_frequency = 440;
  float frequency_factor = pow(2.f, 1.f / 12.f);
  for (int i = -5 * 12; i < 6 * 12; i++) {
    tone_map[i] = type == 0 ? CreateTone(base_frequency * pow(frequency_factor, i - 9)) : CreateSquareWave(base_frequency * pow(frequency_factor, i - 9));
  }
}

void ToneGenerator::setVolume(float volume) {
  if (stream) {
    stream->setVolume(volume);
  }
}

float ToneGenerator::getVolume() const {
  if (stream) {
    return stream->getVolume();
  }
  return 0;
}

void ToneGenerator::startTone(int pitch) {
  if (stream) {
    stream->stop();
  }
  if (tone_map[pitch]) {
    stream = OpenSound(audio_device, tone_map[pitch]);
    if (stream) {
      stream->play();
    }
  }
}

void ToneGenerator::stopTone() {
  if (stream) {
    stream->stop();
  }
}
