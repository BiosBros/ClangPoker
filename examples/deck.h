// deck.h
#ifndef DECK_H
#define DECK_H

#include "card.h"

#define UNO_DECK_SIZE 108

typedef struct {
    Card cards[UNO_DECK_SIZE];
    int top;   // index of the next card to draw
} Deck;

typedef struct {
    Card cards[UNO_DECK_SIZE];
    int top;   // Index of the next open slot (also = # of cards in discard)
} DiscardPile; // по названию думаю понято


void createUnoDeck(Deck *deck);

void shuffleDeck(Deck *deck);

Card drawCard(Deck *deck, DiscardPile *discard);


void initDiscardPile(DiscardPile *dp);

void discardCard(DiscardPile *dp, Card c);

void refillDrawPile(Deck *draw, DiscardPile *discard);

#endif
