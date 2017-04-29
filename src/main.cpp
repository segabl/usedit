/*
 * main.cpp
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#include "ResourceManager.h"
#include "Song.h"
#include "ToneGenerator.h"
#include "TrackHandler.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <cassert>
#include <cmath>
#include <regex>

using namespace std;
using namespace sf;

#define INTERFACE_HEIGHT 128

typedef unique_ptr<TrackHandler> TrackHandlerPtr;

int main(int argc, char* argv[]) {
  ResourceManager::initializeResources(regex_replace(argv[0], regex(R"([^/\\]+$)"), ""));

  RenderWindow win(VideoMode(1280, 720), "USE");
  win.setFramerateLimit(0);
  win.setVerticalSyncEnabled(false);

  Vector2f render_size(win.getSize().x, (win.getSize().y - INTERFACE_HEIGHT));

  Song song;
  ToneGenerator tone_generator(0);

  map<int, TrackHandlerPtr> track_handlers;

  float scale = 1;

  bool song_loaded = argc > 1 ? song.loadFromFile(argv[1 + rand() % (argc - 1)]) : false;
  if (song_loaded) {
    song.fixPitches();

    render_size = Vector2f(win.getSize().x, (win.getSize().y - INTERFACE_HEIGHT) / song.note_tracks.size());
    for (auto track : song.note_tracks) {
      track_handlers[track.first].reset(new TrackHandler(&song, track.first, render_size));
    }

    song.play();
  }

  Clock c;
  float delta;
  while (win.isOpen()) {
    delta = c.restart().asSeconds();

    win.clear(Color(40, 40, 40));

    if (song_loaded) {
      float offset = 0;
      for (auto track : song.note_tracks) {
        track_handlers[track.first]->update(delta, Vector2f((64 / (song.bpm / 60.f)) * scale, 64 * scale * 0.25));
        Sprite s(track_handlers[track.first]->getTexture());
        s.setPosition(0, INTERFACE_HEIGHT + offset);
        win.draw(s);
        RectangleShape r(Vector2f(win.getSize().x, 1));
        r.setPosition(0, INTERFACE_HEIGHT + offset);
        win.draw(r);
        offset += render_size.y;
      }
    }

    RectangleShape r(Vector2f(win.getSize().x, INTERFACE_HEIGHT));
    r.setFillColor(Color(40, 40, 40));
    r.setOutlineThickness(1);
    r.setPosition(0, 0);
    win.draw(r);

    r.setSize(Vector2f(INTERFACE_HEIGHT - 16, INTERFACE_HEIGHT - 16));
    r.setTexture(&song.cover, true);
    r.setPosition(8, 8);
    r.setFillColor(Color::White);
    r.setOutlineThickness(0);
    win.draw(r);

    Text t(song.tags["ARTIST"], ResourceManager::font("default"), 32);
    t.setPosition(8 + INTERFACE_HEIGHT, INTERFACE_HEIGHT * 0.5 - 32 - 8);
    win.draw(t);
    t.setString(song.tags["TITLE"]);
    t.setCharacterSize(24);
    t.setPosition(8 + INTERFACE_HEIGHT, INTERFACE_HEIGHT * 0.5 + 8);
    win.draw(t);

    win.display();

    // Event Logic
    Event e;
    if (win.pollEvent(e)) {
      switch (e.type) {
        case Event::Closed:
          win.close();
          break;
        case Event::Resized: {
          render_size = Vector2f(e.size.width, (e.size.height - INTERFACE_HEIGHT) / song.note_tracks.size());
          for (auto track : song.note_tracks) {
            track_handlers[track.first]->resize(render_size);
          }
          win.setView(View(FloatRect(0, 0, e.size.width, e.size.height)));
          break;
        }
        case Event::MouseWheelMoved:
          scale += 0.25 * e.mouseWheel.delta;
          scale = max(0.25f, scale);
          break;
        case Event::KeyPressed:
          switch (e.key.code) {
            case Keyboard::P:
              if (song_loaded) {
                if (song.isPlaying()) {
                  song.pause();
                } else {
                  if (!song.isPaused()) {
                    song.setPosition(Time::Zero);
                  }
                  song.play();
                }
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }
  }
  return 0;
}
