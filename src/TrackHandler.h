/*
 * TrackHandler.h
 *
 *  Created on: 29.04.2017
 *      Author: Sebastian
 */

#ifndef SRC_TRACKHANDLER_H_
#define SRC_TRACKHANDLER_H_

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

#include "Note.h"
#include "ToneGenerator.h"

#include <list>

typedef std::list<Note> NoteList;

class Song;

class TrackHandler {
  private:
    static sf::Color track_colors[];
    Song* song;
    int track_number;
    ToneGenerator tone_generator;
    sf::RenderTexture texture;
    sf::View view;
    NoteList notes;
    NoteList::iterator current_note;
    NoteList::iterator current_line_start;
    sf::Vector2f scroll_pos;
    sf::Vector2f scroll_to;
  public:
    TrackHandler(Song* song, int track_number, sf::Vector2f size);
    ~TrackHandler();

    void resize(sf::Vector2f size);
    void update(float delta, sf::Vector2f scale, sf::Vector2i mouse_pos, bool allow_input);

    const sf::Texture& getTexture() const;
};

#endif /* SRC_TRACKHANDLER_H_ */
