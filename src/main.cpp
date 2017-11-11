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
#include "gui/DropdownList.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <regex>
#include <vector>

using namespace std;
using namespace sf;
using namespace gui;

#define INTERFACE_HEIGHT      128
#define STATUS_HEIGHT         32
#define SEEK_HEIGHT           8
#define STATUS_TEXT_SIZE      20
#define ICON_TECTURE_SIZE     32

typedef shared_ptr<TrackHandler> TrackHandlerPtr;
typedef shared_ptr<Element> ElementPtr;

void setupLuaState(sol::state& lua);

int main(int argc, char* argv[]) {
  string main_directory = regex_replace(regex_replace(argv[0], regex(R"([^/\\]+$)"), ""), regex(R"(\\)"), "/");
  ResourceManager::initializeResources(main_directory);

  sol::state lua;
  setupLuaState(lua);

  RenderWindow win(VideoMode(1280, 720), "USE");
  win.setFramerateLimit(0);
  win.setVerticalSyncEnabled(false);

  Vector2f render_size(win.getSize().x, (win.getSize().y - INTERFACE_HEIGHT - STATUS_HEIGHT));

  Song song;
  map<int, TrackHandlerPtr> track_handlers;

  float scale = 1;

  Container bottom_elements(win, 0);

  DropdownList list_file(win, Text("File", ResourceManager::font("default"), STATUS_TEXT_SIZE), Vector2f(160, STATUS_HEIGHT), DropdownList::UP);
  bottom_elements.addElement(&list_file);

  Button button_load(win, Text("Open", ResourceManager::font("default"), STATUS_TEXT_SIZE), Vector2f(160, STATUS_HEIGHT));
  list_file.addElement(&button_load);
  Button button_reload(win, Text("Reload", ResourceManager::font("default"), STATUS_TEXT_SIZE), Vector2f(160, STATUS_HEIGHT), false);
  list_file.addElement(&button_reload);
  Button button_save(win, Text("Save", ResourceManager::font("default"), STATUS_TEXT_SIZE), Vector2f(160, STATUS_HEIGHT), false);
  list_file.addElement(&button_save);

  DropdownList list_functions(win, Text("Functions", ResourceManager::font("default"), STATUS_TEXT_SIZE), Vector2f(160, STATUS_HEIGHT), DropdownList::UP, false);
  bottom_elements.addElement(&list_functions);

  vector<string> files = findFiles(main_directory + "functions", R"(\.lua$)");
  vector<ElementPtr> function_elements;
  for (auto file : files) {
    smatch match;
    regex_search(file, match, std::regex(R"(.*?([^/\\]+)\.lua$)", std::regex::icase));
    ElementPtr button(new Button(win, Text(regex_replace(match.str(1), regex(R"(_)"), " "), ResourceManager::font("default"), STATUS_TEXT_SIZE), Vector2f(256, STATUS_HEIGHT)));
    button->setCallback([&song,&lua,file](Element*) {
      song.executeLuaFile(lua, file);
    });
    function_elements.push_back(button);
    list_functions.addElement(button.get());
  }

  function<void(string)> loadSong = [&](string fname) {
    track_handlers.clear();
    bool loaded = song.loadFromFile(fname);
    if (loaded) {
      render_size = Vector2f(win.getSize().x, (win.getSize().y - INTERFACE_HEIGHT - SEEK_HEIGHT - STATUS_HEIGHT) / song.note_tracks.size());
      for (auto track : song.note_tracks) {
        track_handlers[track.first].reset(new TrackHandler(&song, track.first, render_size));
      }
      win.setTitle("USE - " + song.tags["ARTIST"] + " - " + song.tags["TITLE"]);
      song.play();
    } else {
      win.setTitle("USE");
    }
    list_functions.setEnabled(loaded && !function_elements.empty());
    button_save.setEnabled(loaded);
    button_reload.setEnabled(loaded);
  };

  button_load.setCallback([&](Element*) {
    string fname = getOpenFile("Select a file");
    if (fname != "") {
      loadSong(fname);
    }
  });

  button_reload.setCallback([&](Element*) {
    loadSong(song.fname);
  });

  button_save.setCallback([&](Element*) {
    if (rename(song.fname.c_str(), (song.fname + ".backup").c_str()) != 0) {
      log(1, "Could not backup old song file!");
    }
    song.saveToFile(song.fname);
  });

  if (argc > 1) {
    loadSong(argv[1]);
  }

  Clock c;
  float delta;
  while (win.isOpen()) {
    delta = c.restart().asSeconds();

    Vector2i mouse_pos = Mouse::getPosition(win);
    Vector2i win_size = Vector2i(win.getSize());
    Vector2f scale_vec((64 / (song.bpm / 60.f)) * scale, 64 * scale * 0.25);

    win.clear(ResourceManager::color("background"));

    if (song.isLoaded()) {
      float offset = 0;
      RectangleShape r(Vector2f(win_size.x, 2));
      r.setFillColor(ResourceManager::color("interface"));
      for (auto track : track_handlers) {
        track.second->update(delta, scale_vec, Vector2i(mouse_pos.x, mouse_pos.y - INTERFACE_HEIGHT - offset), false);
        Sprite s(track.second->getTexture());
        s.setPosition(0, INTERFACE_HEIGHT + offset);
        win.draw(s);
        if (offset) {
          r.setPosition(0, INTERFACE_HEIGHT + offset - 1);
          win.draw(r);
        }
        offset += render_size.y;
      }
    }

    RectangleShape r(Vector2f(win_size.x, INTERFACE_HEIGHT));
    r.setFillColor(ResourceManager::color("interface"));
    r.setOutlineThickness(0);
    r.setPosition(0, 0);
    win.draw(r);

    r.setSize(Vector2f(win_size.x, STATUS_HEIGHT));
    r.setPosition(0, win_size.y - STATUS_HEIGHT);
    win.draw(r);

    r.setSize(Vector2f(win_size.x * (song.getPosition() / song.length()), SEEK_HEIGHT));
    r.setFillColor(ResourceManager::color("text"));
    r.setOutlineThickness(0);
    r.setPosition(0, win_size.y - STATUS_HEIGHT - SEEK_HEIGHT);
    win.draw(r);
    if (win.hasFocus() && !gui::Element::focusedElement() && mouse_pos.y > win_size.y - STATUS_HEIGHT - SEEK_HEIGHT && mouse_pos.y < win_size.y - STATUS_HEIGHT
        && Mouse::isButtonPressed(Mouse::Button::Left)) {
      song.setPosition(seconds(song.length().asSeconds() * ((float) mouse_pos.x / win_size.x)));
    }

    r.setSize(Vector2f(INTERFACE_HEIGHT - 16, INTERFACE_HEIGHT - 16));
    r.setTexture(&song.cover, true);
    r.setPosition(8, 8);
    r.setFillColor(Color::White);
    r.setOutlineThickness(0);
    win.draw(r);

    Text t(song.isLoaded() ? song.tags["TITLE"] : "No song loaded", ResourceManager::font("default"), 32);
    t.setPosition(8 + INTERFACE_HEIGHT, INTERFACE_HEIGHT * 0.5 - 48 - 8);
    win.draw(t);
    t.setString(song.tags["ARTIST"]);
    t.setCharacterSize(24);
    t.setPosition(8 + INTERFACE_HEIGHT, INTERFACE_HEIGHT * 0.5 - 16);
    win.draw(t);

    r.setTexture(&ResourceManager::texture("icons"), true);
    // Has golden notes
    r.setSize(Vector2f(32, 32));
    r.setPosition(8 + INTERFACE_HEIGHT, INTERFACE_HEIGHT * 0.5 + 20);
    r.setFillColor(Color(255, 255, 255, 55 + 200 * song.hasGoldenNotes()));
    r.setTextureRect(IntRect(0, 0, ICON_TECTURE_SIZE, ICON_TECTURE_SIZE));
    win.draw(r);
    // Has background
    r.setPosition(8 + INTERFACE_HEIGHT + 40, INTERFACE_HEIGHT * 0.5 + 20);
    r.setFillColor(Color(255, 255, 255, 55 + 200 * song.hasBackground()));
    r.setTextureRect(IntRect(ICON_TECTURE_SIZE, 0, ICON_TECTURE_SIZE, ICON_TECTURE_SIZE));
    win.draw(r);
    // Has video
    r.setPosition(8 + INTERFACE_HEIGHT + 40 * 2, INTERFACE_HEIGHT * 0.5 + 20);
    r.setFillColor(Color(255, 255, 255, 55 + 200 * song.hasVideo()));
    r.setTextureRect(IntRect(ICON_TECTURE_SIZE * 2, 0, ICON_TECTURE_SIZE, ICON_TECTURE_SIZE));
    win.draw(r);
    // Has medley
    r.setPosition(8 + INTERFACE_HEIGHT + 40 * 3, INTERFACE_HEIGHT * 0.5 + 20);
    r.setFillColor(Color(255, 255, 255, 55 + 200 * song.hasMedley()));
    r.setTextureRect(IntRect(ICON_TECTURE_SIZE * 3, 0, ICON_TECTURE_SIZE, ICON_TECTURE_SIZE));
    win.draw(r);

    t.setString(toString(song.bpm));
    t.setCharacterSize(24);
    t.setPosition(win_size.x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 - 24 - 4);
    win.draw(t);

    t.setString(toString(song.gap));
    t.setPosition(win_size.x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 + 18 + 4);
    win.draw(t);

    t.setString("bpm");
    t.setCharacterSize(18);
    t.setPosition(win_size.x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 - 24 - 18 - 4);
    win.draw(t);

    t.setString("gap");
    t.setPosition(win_size.x - t.getLocalBounds().width - 16, INTERFACE_HEIGHT / 2 + 4);
    win.draw(t);

    bottom_elements.setPosition(0, win_size.y - STATUS_HEIGHT);
    bottom_elements.update();
    win.draw(bottom_elements);

    win.display();

    // Event Logic
    Event e;
    if (win.pollEvent(e)) {
      switch (e.type) {
        case Event::Closed:
          win.close();
          break;
        case Event::Resized: {
          render_size = Vector2f(e.size.width, (e.size.height - INTERFACE_HEIGHT - SEEK_HEIGHT - STATUS_HEIGHT) / max((int) song.note_tracks.size(), 1));
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
              if (song.isLoaded()) {
                if (song.isPlaying()) {
                  song.pause();
                } else {
                  if (!song.isPaused() && song.getPosition() >= song.length()) {
                    song.setPosition(Time::Zero);
                  }
                  song.play();
                }
              }
              break;
            case Keyboard::Num9:
              song.gap -= e.key.shift ? 10 : 1;
              song.gap = floor(song.gap);
              break;
            case Keyboard::Num0:
              song.gap += e.key.shift ? 10 : 1;
              song.gap = floor(song.gap);
              break;
            case Keyboard::LBracket:
              song.bpm -= e.key.shift ? 1 : 0.1f;
              song.bpm = floor(song.bpm * 10) / 10;
              break;
            case Keyboard::RBracket:
              song.bpm += e.key.shift ? 1 : 0.1f;
              song.bpm = floor(song.bpm * 10) / 10;
              break;
            default:
              log(0, "Key pressed: " + toString(e.key.code));
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

inline void setupLuaState(sol::state& lua) {
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
