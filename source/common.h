#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

typedef enum { COL_RED, COL_YELLOW, COL_GREEN, COL_BLUE, COL_WILD } Color;

typedef enum {
  VAL_0,
  VAL_1,
  VAL_2,
  VAL_3,
  VAL_4,
  VAL_5,
  VAL_6,
  VAL_7,
  VAL_8,
  VAL_9,
  VAL_SKIP,
  VAL_REVERSE,
  VAL_DRAW_TWO,
  VAL_WILD,
  VAL_WILD_DRAW_FOUR
} Value;

typedef struct {
  Color color;
  Value value;
} Card;

typedef struct {
  char name[50];
  int cards_count;
  int score;
  bool is_current;
  bool connected;
} PlayerInfo;

#endif // COMMON_H
