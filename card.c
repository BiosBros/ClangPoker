#include "card.h"
#include <stdio.h>

int main(void) {
  Card karta;
  karta.rank = two;
  karta.suit = clubs;

  printf("karta rank: %d\nkarta suit: %d\n", karta.rank, karta.suit);
}
