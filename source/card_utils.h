#ifndef CARD_UTILS_H
#define CARD_UTILS_H

#include "common.h"

Color parse_color(const char *color_str);
Value parse_value(const char *value_str);
const char *color_to_string(Color color);
const char *value_to_string(Value value);
bool can_play_card(Card player_card, Color current_color, Card top_card);

#endif
