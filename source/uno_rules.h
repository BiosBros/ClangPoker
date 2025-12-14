#ifndef UNO_RULES_H
#define UNO_RULES_H

#include "server.h"

bool can_play_card(Card player_card, Color current_color, Card top_card);

void apply_card_effect(Game *game, Card played_card);

void advance_turn(Game *game);

int next_player_index(Game *game);

void create_uno_deck(Deck *deck);

void shuffle_deck(Deck *deck);

Card draw_card(Deck *deck);

const char *color_to_string(Color color);

const char *value_to_string(Value value);

#endif
