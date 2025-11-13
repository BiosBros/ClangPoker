#ifndef CARD_H
#define CARD_H

#include "stack.h"

enum ranks {
  r_hidden,
  two,
  three,
  four,
  five,
  six,
  seven,
  eight,
  nine,
  ten,
  jack,
  queen,
  king,
  ace
};

enum suits { s_hidden, spades, hearts, diamonds, clubs };

typedef struct {
  enum ranks rank;
  enum suits suit;
} Card;

char *card_to_string(Card *c);

stack *generate_new_deck();

#endif
