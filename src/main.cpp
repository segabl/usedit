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
#include "Utils.h"
#include "gui/Button.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <cassert>
#include <cmath>
#include <regex>
#include <vector>

using namespace std;
using namespace sf;
using namespace gui;

#define INTERFACE_HEIGHT 128
#define STATUS_HEIGHT 32

typedef unique_ptr<TrackHandler> TrackHandlerPtr;
typedef unique_ptr<Button> ButtonPtr;

int main(int argc, char* argv[]) {
  ResourceManager::initializeResources(regex_replace(argv[0], regex(R"([^/\\]+$)"), ""));

  RenderWindow win(VideoMode(1280, 720), "USE");
  win.setFramerateLimit(0);
  win.setVerticalSyncEnabled(false);

  Vector2f render_size(win.getSize().x, (win.getSize().y - INTERFACE_HEIGHT - STATUS_HEIGHT));

  Song song;
  bool song_loaded = false;
  ToneGenerator tone_generator(0);

  map<int, TrackHandlerPtr> track_handlers;

  float scale = 1;

  vector<Button*> buttons;
  Button button_load(Text("Load", ResourceManager::font("default"), STATUS_HEIGHT - 8), Vector2f(160, STATUS_HEIGHT));
  buttons.push_back(&button_load);
  Button button_save(Text("Save", ResourceManager::font("default"), STATUS_HEIGHT - 8), Vector2f(160, STATUS_HEIGHT));
  buttons.push_back(&button_save);

  function<void(string)> loadSong = [&](string fname) {
    song_loaded = song.loadFromFile(fname);
    if (song_loaded) {
      song.fixPitches();

      render_size = Vector2f(win.getSize().x, (win.getSize().y - INTERFACE_HEIGHT - STATUS_HEIGHT) / song.note_tracks.size());
      for (auto track : song.note_tracks) {
        track_handlers[track.first].reset(new TrackHandler(&song, track.first, render_size));
      }

      song.play();
    }
    button_save.setEnabled(song_loaded);
  };

  button_load.setCallback([&](Element*) {
    string fname = getOpenFile("Select a file");
    if (fname != "") {
      loadSong(fname);
    }
  });

  if (argc > 1) {
    loadSong(argv[1]);
  }

  Clock c;
  float delta;
  while (win.isOpen()) {
    delta = c.restart().asSeconds();

    Vector2i mouse_pos = Mouse::getPosition(win);
    Vector2f scale_vec((64 / (song.bpm / 60.f)) * scale, 64 * scale * 0.25);

    win.clear(Color(40, 40, 40));

    if (song_loaded) {
      float offset = 0;
      for (auto track : song.note_tracks) {
        track_handlers[track.first]->update(delta, scale_vec, Vector2i(mouse_pos.x, mouse_pos.y - INTERFACE_HEIGHT - offset), !gui::Element::focusedElement());
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

    t.setString(toString(song.bpm));
    t.setCharacterSize(24);
    t.setPosition(win.getSize().x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 - 24 - 4);
    win.draw(t);

    t.setString(toString(song.gap));
    t.setPosition(win.getSize().x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 + 18 + 4);
    win.draw(t);

    t.setString("bpm");
    t.setCharacterSize(18);
    t.setPosition(win.getSize().x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 - 24 - 18 - 4);
    win.draw(t);

    t.setString("gap");
    t.setPosition(win.getSize().x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 + 4);
    win.draw(t);

    r.setSize(Vector2f(win.getSize().x, 1));
    r.setTexture(nullptr);
    r.setPosition(0, win.getSize().y - STATUS_HEIGHT - 1);
    win.draw(r);

    for (size_t i = 0; i < buttons.size(); i++) {
      buttons[i]->setPosition(win.getSize().x / max((size_t) 8, buttons.size()) * i, win.getSize().y - STATUS_HEIGHT);
      buttons[i]->setSize(win.getSize().x / max((size_t) 8, buttons.size()), STATUS_HEIGHT);
      buttons[i]->update(mouse_pos);
      win.draw(*buttons[i]);
    }

    win.display();

    // Event Logic
    Event e;
    if (win.pollEvent(e)) {
      switch (e.type) {
        case Event::Closed:
          win.close();
          break;
        case Event::Resized: {
          render_size = Vector2f(e.size.width, (e.size.height - INTERFACE_HEIGHT - STATUS_HEIGHT) / song.note_tracks.size());
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
            case Keyboard::L:
              song.changeNoteLengths(2);
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
