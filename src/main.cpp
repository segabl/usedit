/*
 * main.cpp
 *
 *  Created on: 25.04.2017
 *      Author: Sebastian
 */

#include "ResourceManager.h"
#include "Song.h"
#include "StringUtils.h"
#include "ToneGenerator.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>
#include <regex>

using namespace std;
using namespace sf;

#define INTERFACE_HEIGHT 128

NoteList::iterator findLineEnd(NoteList& l, NoteList::iterator it);

Color colorByTrack(int tracknumber);

void drawNote(RenderTarget& rt, Note* note, Vector2f scale, Color blend = Color::White);

int main(int argc, char* argv[]) {
  ResourceManager::initializeResources(regex_replace(argv[0], regex(R"([^/\\]+$)"), ""));

  RenderWindow win(VideoMode(1280, 720), "USE");
  win.setFramerateLimit(0);
  win.setVerticalSyncEnabled(false);

  RenderTexture tex_render;
  Vector2f render_size(win.getSize().x, (win.getSize().y - INTERFACE_HEIGHT));
  tex_render.create(render_size.x, render_size.y);
  View render_view(FloatRect(0, 0, render_size.x, render_size.y));
  View main_view(FloatRect(0, 0, win.getSize().x, win.getSize().y));

  Song song;
  ToneGenerator tone_generator(0);

  NoteListMap::iterator track;
  NoteList::iterator it_current;
  NoteList::iterator it_next;
  NoteList::iterator it_line_start;
  NoteList::iterator it_line_end;

  Vector2f scale(24, 24);
  Vector2f scroll_pos(0, 0);
  Vector2f scroll_to(0, 0);
  float song_pos = 0;

  bool song_loaded = argc > 1 ? song.loadFromFile(argv[1]) : false;
  if (song_loaded) {
    song.fixPitches();

    cout << song.tags["ARTIST"] << " - " << song.tags["TITLE"] << endl;
    cout << song.bpm << " BPM, " << song.gap << " GAP" << endl;
    cout << song.note_tracks.size() << " lyric track/s" << endl << endl;

    track = prev(song.note_tracks.end());
    it_current = track->second.end();
    it_next = track->second.begin();
    it_line_start = it_next;
    it_line_end = findLineEnd(track->second, it_line_start);

    scroll_pos.x = (*it_next)->position - 4;
    scroll_pos.y = (*it_next)->pitch;
    scroll_to = scroll_pos;
    song_pos = -1000;

    song.play();
  }

  Clock c;
  float delta;
  while (win.isOpen()) {
    delta = c.restart().asSeconds();

    // Event Logic
    Event e;
    if (win.pollEvent(e)) {
      switch (e.type) {
        case Event::Closed:
          win.close();
          break;
        case Event::Resized: {
          render_size = Vector2f(e.size.width, e.size.height - INTERFACE_HEIGHT);
          tex_render.create(render_size.x, render_size.y);
          render_view = View(FloatRect(0, 0, render_size.x, render_size.y));
          main_view = View(FloatRect(0, 0, e.size.width, e.size.height));
          break;
        }
        case Event::MouseWheelMoved:
          scale.x += 4 * e.mouseWheel.delta;
          scale.y += 4 * e.mouseWheel.delta;
          scale.x = max(4.f, scale.x);
          scale.y = max(4.f, scale.y);
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

    tex_render.clear(Color(20, 20, 20));
    RectangleShape rectangle;

    if (song_loaded) {

      tone_generator.setVolume(1 * song.isPlaying());

      song_pos = (song.bpm / 60.f) * (song.getPosition().asSeconds() - song.gap / 1000.f) * 4.f;

      if (it_next != track->second.end() && song_pos >= (*it_next)->position && it_next != it_current) {
        it_current = it_next;
        if ((*it_next)->type == Note::LINEBREAK) {
          it_line_start = next(it_next);
          it_line_end = findLineEnd(track->second, it_line_start);
        } else {
          // Play tone
          if ((*it_next)->type != Note::FREESTYLE) {
            tone_generator.startTone((*it_next)->pitch);
          }
        }
        it_next++;
      }
      // Stop any previous tones if we passed the note end
      if (it_current != track->second.end() && song_pos > (*it_current)->position + (*it_current)->length) {
        tone_generator.stopTone();
      }

      Color track_color = colorByTrack(track->first);

      Note* note_current = it_current != track->second.end() ? *it_current : nullptr;
      Note* note_next = it_next != track->second.end() ? *it_next : nullptr;
      Note* note_line_start = it_line_start != track->second.end() ? *it_line_start : nullptr;
      Note* note_line_end = it_line_end != track->second.end() ? *it_line_end : nullptr;
      if ((song_pos - scroll_to.x) * scale.x > render_size.x - 4 * scale.x
          && (note_next || (note_current && (note_current->position + note_current->length - 4 - scroll_to.x) * scale.x > render_size.x))) {
        if (note_line_start && (song_pos - note_line_start->position - 4) * scale.x <= render_size.x / 2.f) {
          scroll_to.x = note_line_start->position - 4;
        } else {
          scroll_to.x = floor(song_pos) - 4;
        }
      }
      if (note_current && note_next && note_current->type == Note::LINEBREAK && (note_next->position - scroll_to.x) * scale.x > render_size.x) {
        scroll_to.x = note_next->position - 4;
      }
      if (note_current && note_current->type != Note::LINEBREAK && abs(note_current->pitch + scroll_to.y) * scale.y * 0.5f > render_size.y) {
        scroll_to.y = note_current->pitch;
      }
      scroll_to.x = min(scroll_to.x, (*prev(track->second.end()))->position + (*prev(track->second.end()))->length + 4.f);
      scroll_pos.x += (scroll_to.x - scroll_pos.x) * delta * song.bpm * scale.x * 0.001;
      scroll_pos.y += (scroll_to.y - scroll_pos.y) * delta * song.bpm * scale.y * 0.001;

      render_view.setCenter(render_size.x / 2 + scroll_pos.x * scale.x, 0 - scroll_pos.y * scale.y * 0.5);
      tex_render.setView(render_view);

      rectangle.setSize(Vector2f(render_size.x, scale.y));
      rectangle.setFillColor(Color(255, 255, 255, 5));
      for (int i = floor((-scroll_pos.y - render_size.y / scale.y) * 0.5); i < ceil((-scroll_to.y + render_size.y / scale.y) * 0.5f); i++) {
        if (i % 2) {
          continue;
        }
        rectangle.setPosition(render_view.getCenter().x - render_size.x / 2, i * scale.y);
        tex_render.draw(rectangle);
      }

      rectangle.setSize(Vector2f(1, render_size.y));
      for (int i = floor(scroll_pos.x); i < scroll_to.x + render_size.x / scale.x; i++) {
        rectangle.setPosition(i * scale.x, render_view.getCenter().y - render_size.y / 2);
        rectangle.setFillColor(Color(255, 255, 255, 10 + 20 * (i % 4 == 0)));
        tex_render.draw(rectangle);
      }

      Note* note = nullptr;
      string lyrics;
      for (auto it = track->second.begin(); it != track->second.end(); it = next(it)) {
        note = (*it);
        if ((note->position + note->length - scroll_pos.x) * scale.x < 0) {
          continue;
        }
        if ((note->position - scroll_pos.x) * scale.x > render_size.x) {
          break;
        }
        if (note->type == Note::LINEBREAK) {
          rectangle.setSize(Vector2f(1, render_size.y));
          rectangle.setPosition(note->position * scale.x, render_view.getCenter().y - render_size.y / 2);
          rectangle.setFillColor(Color(255, 255, 0, 100));
          tex_render.draw(rectangle);
        } else {
          Color c = (song_pos >= note->position && song_pos <= note->position + note->length) ? track_color : Color::White;
          drawNote(tex_render, note, scale, c);
        }
      }
      rectangle.setSize(Vector2f(1, render_size.y));
      rectangle.setPosition(song_pos * scale.x, render_view.getCenter().y - render_size.y / 2);
      rectangle.setFillColor(track_color);
      tex_render.draw(rectangle);

      tex_render.setView(tex_render.getDefaultView());

      float width = 0;
      list<Text> lyric_list;
      Text t("", ResourceManager::font("lyrics"), 32);
      for (auto it = it_line_start; it != it_line_end; it = next(it)) {
        note = (*it);
        t.setString(note->lyrics);
        t.setPosition(width, render_size.y - 32 - 16);
        Color c = note->type != Note::GOLD ? Color(255, 255, 255, 100 + 155 * (note->type != Note::FREESTYLE)) : Color(255, 255, 100);
        if (song_pos >= note->position && song_pos <= note->position + note->length) {
          c = c * track_color;
        }
        t.setFillColor(c);
        t.setOutlineColor(Color(c.r * 0.5, c.g * 0.5, c.b * 0.5, c.a * 0.5));
        t.setOutlineThickness(1);
        lyric_list.push_back(t);
        width += t.getLocalBounds().width + t.getLocalBounds().left;
      }
      for (auto t : lyric_list) {
        t.setPosition(render_size.x / 2 - width / 2 + t.getPosition().x, t.getPosition().y);
        tex_render.draw(t);
      }
    }
    tex_render.display();

    win.clear(Color(40, 40, 40));
    win.setView(main_view);

    Sprite s(tex_render.getTexture());
    s.setPosition(0, INTERFACE_HEIGHT);
    win.draw(s);

    rectangle.setSize(Vector2f(win.getSize().x, INTERFACE_HEIGHT));
    rectangle.setFillColor(Color(40, 40, 40));
    rectangle.setOutlineThickness(1);
    rectangle.setPosition(0, 0);
    win.draw(rectangle);

    rectangle.setSize(Vector2f(INTERFACE_HEIGHT - 16, INTERFACE_HEIGHT - 16));
    rectangle.setTexture(&song.cover, true);
    rectangle.setPosition(8, 8);
    rectangle.setFillColor(Color::White);
    rectangle.setOutlineThickness(0);
    win.draw(rectangle);

    Text t(song.tags["ARTIST"], ResourceManager::font("default"), 32);
    t.setPosition(8 + INTERFACE_HEIGHT, INTERFACE_HEIGHT * 0.5 - 32 - 8);
    win.draw(t);
    t.setString(song.tags["TITLE"]);
    t.setCharacterSize(24);
    t.setPosition(8 + INTERFACE_HEIGHT, INTERFACE_HEIGHT * 0.5 + 8);
    win.draw(t);

    win.display();
  }
  return 0;
}

NoteList::iterator findLineEnd(NoteList& l, NoteList::iterator it) {
  while (it != l.end() && (*it)->type != Note::LINEBREAK) {
    it++;
  }
  return it;
}

Color colorByTrack(int tracknumber) {
  switch (tracknumber) {
    case 2:
      return Color(255, 100, 100);
    default:
      return Color(70, 220, 255);
  }
}

void drawNote(RenderTarget& rt, Note* note, Vector2f scale, Color blend) {
  Texture tex = ResourceManager::texture("note");
  Vector2u tex_size = tex.getSize();
  VertexArray note_array(PrimitiveType::TriangleStrip, 8);
  note_array[0].texCoords = Vector2f(0, 0);
  note_array[1].texCoords = Vector2f(0, tex_size.y);
  note_array[2].texCoords = Vector2f(tex_size.x, 0);
  note_array[3].texCoords = Vector2f(tex_size.x, tex_size.y);
  note_array[4].texCoords = Vector2f(tex_size.x, 0);
  note_array[5].texCoords = Vector2f(tex_size.x, tex_size.y);
  note_array[6].texCoords = Vector2f(0, 0);
  note_array[7].texCoords = Vector2f(0, tex_size.y);
  Color c = note->type != Note::GOLD ? Color(255, 255, 255, 100 + 155 * (note->type != Note::FREESTYLE)) : Color(255, 255, 100);
  c = c * blend;

  Vector2f pos(note->position * scale.x, -note->pitch * scale.y * 0.5 - scale.y * 0.5);
  float len = note->length * scale.x;
  for (int i = 0; i < 8; i++) {
    note_array[i].color = c;
  }
  note_array[0].position = pos + Vector2f(0, 0);
  note_array[1].position = pos + Vector2f(0, scale.y);
  note_array[2].position = pos + Vector2f(scale.x * 0.5, 0);
  note_array[3].position = pos + Vector2f(scale.x * 0.5, scale.y);
  note_array[4].position = pos + Vector2f(len - scale.x * 0.5, 0);
  note_array[5].position = pos + Vector2f(len - scale.x * 0.5, scale.y);
  note_array[6].position = pos + Vector2f(len, 0);
  note_array[7].position = pos + Vector2f(len, scale.y);

  rt.draw(note_array, &tex);
}