#include <stdio.h>
#include "deck.h"
#include "card.h"

int main() {
    Deck deck;
    createUnoDeck(&deck);
    shuffleDeck(&deck);

    // Test: print the first 20 cards
    for (int i = 0; i < 20; i++) {
        Card c = drawCard(&deck);
        printCard(&c);
    }

    return 0;
}
