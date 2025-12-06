#include "deck.h"
#include "card.h"
void printCard(const Card *c) {
    const char *colors[] = { "Red", "Yellow", "Green", "Blue", "Wild" };
    const char *values[] = {
        "0","1","2","3","4","5","6","7","8","9",
        "Skip","Reverse","Draw Two",
        "Wild","Wild Draw Four"
    };

    printf("%s %s\n", colors[c->color], values[c->value]);
}
