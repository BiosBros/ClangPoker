#include "card.h"
#include "malloc-wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *card_to_string(Card *c) {
  if (!c)
    return NULL;
  char *retval;
  char *card_rank;
  char *card_suit;

  const char *suits_arr[] = {"X", "♠", "♡", "♢", "♣"};
  const char *ranks_arr[] = {"X", "2", "3",  "4", "5", "6", "7",
                             "8", "9", "10", "J", "Q", "K", "A"};
  *card_rank = *ranks_arr[c->rank];
  *card_suit = *suits_arr[c->suit];
  size_t needed = snprintf(NULL, 0, "%s %s", card_suit, card_rank);
  retval = xmalloc(needed);
  snprintf(retval, needed, "%s %s", card_suit, card_rank);
  return retval;
}
