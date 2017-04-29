/*
 * ToneGenerator.h
 *
 *  Created on: 26.04.2017
 *      Author: Sebastian
 */

#ifndef TONEGENERATOR_H_
#define TONEGENERATOR_H_

#include <audiere.h>

#include <map>

using audiere::AudioDevicePtr;
using audiere::OutputStreamPtr;
using audiere::SampleSourcePtr;
using std::map;

typedef map<int, SampleSourcePtr> SampleSourcePtrMap;

class ToneGenerator {
  private:
    static AudioDevicePtr audio_device;
    SampleSourcePtrMap tone_map;
    OutputStreamPtr stream;
  public:
    ToneGenerator(int type = 0);
    void setVolume(float volume);
    float getVolume() const;
    void startTone(int pitch);
    void stopTone();
};

#endif /* TONEGENERATOR_H_ */