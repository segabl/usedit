/*
 * ToneGenerator.h
 *
 *  Created on: 26.04.2017
 *      Author: Sebastian
 */

#ifndef TONEGENERATOR_H_
#define TONEGENERATOR_H_

#include <audiere.h>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <map>
#include <thread>
#include <mutex>
#include <tuple>

typedef std::map<int, audiere::OutputStreamPtr> OutputStreamPtrMap;

class ToneGenerator {
  private:
    static audiere::AudioDevicePtr audio_device;
    OutputStreamPtrMap tone_map;
    audiere::OutputStreamPtr stream;
    int type;
    float volume;
    sf::Time time;
    bool alive;
    sf::Clock clock;
    std::thread play_thread;
    std::mutex mtx;
    void work();
  public:
    ToneGenerator(int type = 0);
    ~ToneGenerator();
    void setVolume(float volume);
    void play(int pitch, sf::Time time = sf::Time::Zero);
    void stop();
    bool isPlaying();
};

#endif /* TONEGENERATOR_H_ */
