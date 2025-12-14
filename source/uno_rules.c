#include "uno_rules.h"

/*
 * Можно ли сыграть карту
 */
bool can_play_card(Card player_card, Color current_color, Card top_card) {
  if (player_card.color == COL_WILD)
    return true;

  if (player_card.color == current_color)
    return true;

  if (player_card.value == top_card.value)
    return true;

  return false;
}

/*
 * Индекс следующего игрока
 */
int next_player_index(Game *game) {
  if (game->direction_clockwise) {
    return (game->current_player + 1) % game->players_count;
  } else {
    return (game->current_player - 1 + game->players_count) %
           game->players_count;
  }
}

/*
 * Применение эффектов карты
 */
void apply_card_effect(Game *game, Card played_card) {
  switch (played_card.value) {
  case VAL_SKIP:
    /*
     * Следующий игрок пропускает ход
     */
    game->current_player = next_player_index(game);
    break;

  case VAL_REVERSE:
    /*
     * Меняем направление
     * В UNO при 2 игроках reverse = skip
     */
    game->direction_clockwise = !game->direction_clockwise;

    if (game->players_count == 2) {
      game->current_player = next_player_index(game);
    }
    break;

  case VAL_DRAW_TWO:
    game->draw_accumulator += 2;
    game->player_to_draw = next_player_index(game);
    break;

  case VAL_WILD_DRAW_FOUR:
    game->draw_accumulator += 4;
    game->player_to_draw = next_player_index(game);
    break;

  default:
    /*
     * Обычные карты — без эффекта
     */
    break;
  }
}

/*
 * Переключение хода
 */
void advance_turn(Game *game) {
  game->players[game->current_player].is_turn = false;
  game->current_player = next_player_index(game);
  game->players[game->current_player].is_turn = true;
}

void create_uno_deck(Deck *deck) {
  int index = 0;

  for (Color c = COL_RED; c <= COL_BLUE; c++) {
    deck->cards[index++] = (Card){c, VAL_0};

    for (Value v = VAL_1; v <= VAL_9; v++) {
      deck->cards[index++] = (Card){c, v};
      deck->cards[index++] = (Card){c, v};
    }

    for (int i = 0; i < 2; i++) {
      deck->cards[index++] = (Card){c, VAL_SKIP};
      deck->cards[index++] = (Card){c, VAL_REVERSE};
      deck->cards[index++] = (Card){c, VAL_DRAW_TWO};
    }
  }

  for (int i = 0; i < 4; i++)
    deck->cards[index++] = (Card){COL_WILD, VAL_WILD};

  for (int i = 0; i < 4; i++)
    deck->cards[index++] = (Card){COL_WILD, VAL_WILD_DRAW_FOUR};

  deck->top = 0;
}

void shuffle_deck(Deck *deck) {
  for (int i = UNO_DECK_SIZE - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    Card temp = deck->cards[i];
    deck->cards[i] = deck->cards[j];
    deck->cards[j] = temp;
  }
}

Card draw_card(Deck *deck) {
  if (deck->top < UNO_DECK_SIZE) {
    return deck->cards[deck->top++];
  }
  return (Card){COL_WILD, VAL_WILD};
}

const char *color_to_string(Color color) {
  switch (color) {
  case COL_RED:
    return "red";
  case COL_YELLOW:
    return "yellow";
  case COL_GREEN:
    return "green";
  case COL_BLUE:
    return "blue";
  case COL_WILD:
    return "wild";
  default:
    return "unknown";
  }
}

const char *value_to_string(Value value) {
  switch (value) {
  case VAL_0:
    return "0";
  case VAL_1:
    return "1";
  case VAL_2:
    return "2";
  case VAL_3:
    return "3";
  case VAL_4:
    return "4";
  case VAL_5:
    return "5";
  case VAL_6:
    return "6";
  case VAL_7:
    return "7";
  case VAL_8:
    return "8";
  case VAL_9:
    return "9";
  case VAL_SKIP:
    return "skip";
  case VAL_REVERSE:
    return "reverse";
  case VAL_DRAW_TWO:
    return "draw_two";
  case VAL_WILD:
    return "wild";
  case VAL_WILD_DRAW_FOUR:
    return "wild_draw_four";
  default:
    return "unknown";
  }
}
