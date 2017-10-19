/*
 * TrackHandler.cpp
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#include "TrackHandler.h"

#include <SFML/Graphics.hpp>
#include <cassert>
#include <cmath>
#include <list>
#include <string>

#include "Note.h"
#include "ResourceManager.h"
#include "Song.h"
#include "Utils.h"

using namespace sf;
using namespace std;

sf::Color TrackHandler::track_colors[] = {
  Color(70, 220, 255), // P1
  Color(70, 220, 255), // P1 duet
  Color(255, 100, 100) // P2 duet
};

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
  note_array[2].position = pos + Vector2f(min(scale.y * 0.5f, len / 2.f), 0);
  note_array[3].position = pos + Vector2f(min(scale.y * 0.5f, len / 2.f), scale.y);
  note_array[4].position = pos + Vector2f(len - min(scale.y * 0.5f, len / 2.f), 0);
  note_array[5].position = pos + Vector2f(len - min(scale.y * 0.5f, len / 2.f), scale.y);
  note_array[6].position = pos + Vector2f(len, 0);
  note_array[7].position = pos + Vector2f(len, scale.y);

  rt.draw(note_array, &tex);
}

TrackHandler::TrackHandler(Song* song, int track_number, Vector2f size) :
    song(song), track_number(track_number), tone_generator(abs(track_number % 2)), view(FloatRect(0, 0, size.x, size.y)), notes(song->note_tracks[track_number]), current_note(
        notes.begin()), scroll_pos(current_note->position - 4, current_note->pitch), scroll_to(scroll_pos) {
  texture.create(size.x, size.y);
  log(0, "Track handler " + toString(track_number) + " created");
}

TrackHandler::~TrackHandler() {
  log(0, "Track handler " + toString(track_number) + " destroyed");
}

void TrackHandler::resize(Vector2f size) {
  texture.create(size.x, size.y);
  view.setSize(size.x, size.y);
}

void TrackHandler::update(float delta, Vector2f scale, Vector2i mouse_pos, bool allow_input) {
  texture.clear(ResourceManager::color("background"));
  RectangleShape rectangle;

  float song_pos = SECONDS_TO_BEATS(song->getPosition().asSeconds() - song->gap / 1000.f, song->bpm);

  if (song_pos >= current_note->position && song_pos < current_note->position + current_note->length && current_note->type != Note::FREESTYLE
      && current_note->type != Note::LINEBREAK && song->isPlaying() && !tone_generator.isPlaying()) {
    tone_generator.play(current_note->pitch, seconds(BEATS_TO_SECONDS(current_note->length - song_pos + current_note->position, song->bpm));
  }

  if (current_note->next && song_pos > current_note->position + current_note->length) {
    current_note = current_note->next;
  }

  assert(current_note);
  assert(current_note->line_start);
  assert(current_note->line_end);

  Vector2f track_size(texture.getSize().x, texture.getSize().y);

  bool fixed_scroll = track_number > 0;
  if (fixed_scroll) {
    if ((current_note->line_start->position - song_pos) * scale.x > track_size.x * 2.f) {
      scroll_to.x = current_note->line_start->position - (track_size.x / scale.x) * 0.25f;
    } else {
      scroll_to.x = max(song_pos - (track_size.x / scale.x) * 0.25f, scroll_to.x);
    }
    scroll_to.x = min(scroll_to.x, (float) notes.end()->position + notes.end()->length);
  } else {
    if ((song_pos - scroll_to.x) * scale.x > track_size.x * 0.9f && current_note->next) {
      scroll_to.x = floor(song_pos);
    }
    if (current_note->next && current_note->type == Note::LINEBREAK && (current_note->next->position - scroll_to.x) * scale.x > track_size.x) {
      scroll_to.x = current_note->next->position - 4;
    }
    scroll_to.x = min(scroll_to.x, notes.end()->position + notes.end()->length + 4.f);
  }
  if (current_note->type != Note::LINEBREAK && abs(current_note->pitch - scroll_to.y) * scale.y + scale.y > track_size.y) {
    scroll_to.y = (scroll_to.y + current_note->pitch) / 2;
  }
  scroll_pos.x += (scroll_to.x - scroll_pos.x) * delta * scale.x * 0.75;
  scroll_pos.y += (scroll_to.y - scroll_pos.y) * delta * scale.y * 0.75;

  view.setCenter(track_size.x / 2 + scroll_pos.x * scale.x, 0 - scroll_pos.y * scale.y * 0.5);
  texture.setView(view);

  rectangle.setSize(Vector2f(track_size.x, scale.y));
  rectangle.setFillColor(Color(255, 255, 255, 5));
  for (int i = floor((-scroll_pos.y - track_size.y / scale.y) * 0.5); i < ceil((-scroll_to.y + track_size.y / scale.y) * 0.5f); i++) {
    if (i % 2) {
      continue;
    }
    rectangle.setPosition(view.getCenter().x - view.getSize().x / 2, i * scale.y);
    texture.draw(rectangle);
  }

  rectangle.setSize(Vector2f(1, track_size.y));
  for (int i = floor(scroll_pos.x); i < scroll_to.x + track_size.x / scale.x; i++) {
    rectangle.setPosition(i * scale.x, view.getCenter().y - view.getSize().y / 2);
    rectangle.setFillColor(Color(255, 255, 255, 10 + 20 * (i % 4 == 0)));
    texture.draw(rectangle);
  }

  string lyrics;
  for (Note* note = notes.begin(); note; note = note->next) {
    if ((note->position + note->length - scroll_pos.x) * scale.x < 0) {
      continue;
    }
    if ((note->position - scroll_pos.x) * scale.x > track_size.x) {
      break;
    }
    if (note->type == Note::LINEBREAK) {
      rectangle.setSize(Vector2f(1, track_size.y));
      rectangle.setPosition(note->position * scale.x, view.getCenter().y - view.getSize().y / 2);
      rectangle.setFillColor(Color(255, 255, 0, 100));
      texture.draw(rectangle);
    } else {
      Color c = (song_pos >= note->position && song_pos <= note->position + note->length) ? track_colors[abs(track_number % 3)] : Color::White;
      drawNote(texture, note, scale, c);
    }
  }
  rectangle.setSize(Vector2f(1, track_size.y));
  if (fixed_scroll) {
    rectangle.setPosition(min(song_pos * scale.x, view.getCenter().x - view.getSize().x / 2 + track_size.x * 0.25f), view.getCenter().y - view.getSize().y / 2);
  } else {
    rectangle.setPosition(song_pos * scale.x, view.getCenter().y - view.getSize().y / 2);
  }
  rectangle.setFillColor(track_colors[abs(track_number % 3)]);
  texture.draw(rectangle);

  texture.setView(texture.getDefaultView());

  float width = 0;
  list<Text> lyric_list;
  Text t("", ResourceManager::font("lyrics"), 32);
  for (Note* note = current_note->line_start; note && note->type != Note::LINEBREAK; note = note->next) {
    t.setString(note->lyrics);
    t.setPosition(width, track_size.y - 32 - 16);
    Color c = note->type != Note::GOLD ? Color(255, 255, 255, 100 + 155 * (note->type != Note::FREESTYLE)) : Color(255, 255, 100);
    if (song_pos >= note->position && song_pos <= note->position + note->length) {
      c = c * track_colors[abs(track_number % 3)];
    }
    t.setFillColor(c);
    t.setOutlineColor(Color(c.r * 0.5, c.g * 0.5, c.b * 0.5, c.a * 0.5));
    t.setOutlineThickness(1);
    lyric_list.push_back(t);
    width += t.getLocalBounds().width + t.getLocalBounds().left;
  }
  for (auto t : lyric_list) {
    t.setPosition(track_size.x / 2 - width / 2 + t.getPosition().x, t.getPosition().y);
    texture.draw(t);
  }

  texture.display();
}

const Texture& TrackHandler::getTexture() const {
  return texture.getTexture();
}
