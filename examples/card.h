// card.h
#ifndef CARD_H
#define CARD_H

// ----- COLOR ENUM -----
typedef enum {
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_WILD  // Only for wild cards
} Color;

// ----- VALUE / TYPE ENUM -----
typedef enum {
    // Number cards
    VALUE_0, VALUE_1, VALUE_2, VALUE_3, VALUE_4,
    VALUE_5, VALUE_6, VALUE_7, VALUE_8, VALUE_9,

    // Action cards
    VALUE_SKIP,
    VALUE_REVERSE,
    VALUE_DRAW_TWO,

    // Wilds
    VALUE_WILD,
    VALUE_WILD_DRAW_FOUR
} Value;

// ----- CARD STRUCT -----
typedef struct {
    Color color;
    Value value;
} Card;

#endif
